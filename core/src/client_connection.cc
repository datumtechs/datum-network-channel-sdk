// file client_connection.cc
#include "client_connection.h"
#include <thread>
#include <chrono>   
using namespace chrono;

ClientConnection::ClientConnection(const string& nodeid):nodeid_(nodeid){}

bool ClientConnection::write(const string& msgid, const bytes& data)
{
  std::unique_lock<std::mutex> lck(mutex_);
  auto iter = map_queue_.find(msgid);

  if(iter == map_queue_.end())
  {
    shared_ptr<queue<bytes>> ptr_data_queue_ = make_shared<queue<bytes>>();
    ptr_data_queue_->push(data);
    map_queue_.insert(std::pair<string,  shared_ptr<queue<bytes>>>(msgid, ptr_data_queue_));
  }
  else
  {
    iter->second->push(data);
  }
  cv_.notify_all();
  return true;
}

ssize_t ClientConnection::recv(const string& msgid, char* data, uint64_t length, int64_t timeout)
{
  if (timeout < 0)
    timeout = 10 * 1000000;
  
  auto beg = system_clock::now();
  int64_t elapsed = 0;
  int64_t remain_time = timeout;
  do {
    std::unique_lock<std::mutex> lck(mutex_);
    auto iter = map_queue_.find(msgid);
    if(iter != map_queue_.end() && !iter->second->empty())
    {
      const bytes& bytes_data = iter->second->front();
      uint64_t nSize = bytes_data.size();
      copy(bytes_data.begin(), bytes_data.end(), data);
      iter->second->pop();
      return nSize;
    }

    auto end = system_clock::now();
    elapsed = duration_cast<duration<int64_t, std::milli>>(end - beg).count();
    remain_time = timeout - elapsed;

    if(std::cv_status::timeout == cv_.wait_for(lck, std::chrono::milliseconds(remain_time)))
    {
      return 0;
    }
  } while(true);
  
  return 0;
}
