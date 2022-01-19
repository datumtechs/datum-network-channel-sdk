// file client_connection.cc
#include "client_connection.h"
#include <thread>
#include <chrono>   
using namespace chrono;

ClientConnection::ClientConnection(const string& nodeid):nodeid_(nodeid){}

bool ClientConnection::write(const string& msgid, const bytes& data)
{
  std::unique_lock<std::mutex> lck(mutex_);
  auto iter = mapbuffer_.find(msgid);
  if(iter == mapbuffer_.end())
  {
#if USE_BUFFER
     mapbuffer_.emplace(msgid, make_shared<cycle_buffer>(1024 * 8));
#else
    mapbuffer_.emplace(msgid, make_shared<queue<bytes>>());
#endif
  }

#if USE_BUFFER
  mapbuffer_[msgid]->write((const char*)data.data(), data.size());
#else
  mapbuffer_[msgid]->emplace(data);
#endif

  cv_.notify_all();
  return true;
}

ssize_t ClientConnection::recv(const string& msgid, char* data, uint64_t length, int64_t timeout)
{
  // if (timeout < 0)
  //   timeout = 10 * 1000000;
  
  auto beg = system_clock::now();
  int64_t elapsed = 0;
  int64_t remain_time = recv_timeout_;
  do {
    std::unique_lock<std::mutex> lck(mutex_);
    auto iter = mapbuffer_.find(msgid);
    if(iter != mapbuffer_.end() && !iter->second->empty()) {
  #if USE_BUFFER
      shared_ptr<cycle_buffer> buffer = nullptr;
      buffer = iter->second;
      if (buffer->can_read(length)) { // got data
        return buffer->read(data, length);
      }
  #else
      const bytes& bytes_data = iter->second->front();
      uint64_t nSize = bytes_data.size();
      copy(bytes_data.begin(), bytes_data.end(), data);
      iter->second->pop();
      return nSize;
  #endif
    }
    
    auto end = system_clock::now();
    elapsed = duration_cast<duration<int64_t, std::milli>>(end - beg).count();
    remain_time = recv_timeout_ - elapsed;

    if(std::cv_status::timeout == cv_.wait_for(lck, std::chrono::milliseconds(remain_time)))
    {
      return 0;
    }
  } while(true);
  
  return 0;
}
