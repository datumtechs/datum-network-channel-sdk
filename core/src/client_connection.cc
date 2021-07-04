// file client_connection.cc
#include "client_connection.h"
#include <thread>
#include <chrono>   
using namespace chrono;

ClientConnection::ClientConnection() 
{
  buffer_ = make_shared<cycle_buffer>(1024 * 1024 * 50);
}

ssize_t ClientConnection::recv(const string& id, char* data, uint64_t length, int64_t timeout) 
{
  // cout << "ClientConnection::recv, msgid:" << id << ", buff size:" << buffer_->size() << endl;
  if (timeout < 0)
    timeout = 1000 * 1000000;

  int64_t elapsed = 0;
  auto beg = system_clock::now();
  ssize_t ret = 0;
  bool retry = false;
  do {
    auto end = system_clock::now();
    elapsed = duration_cast<duration<int64_t, std::milli>>(end - beg).count();
  
    if (elapsed > timeout) {
      return 0; // timeout
    }

    if (retry)
      std::this_thread::yield();

    unique_lock<mutex> lck(mapbuffer_mtx_);

    auto iter = mapbuffer_.find(id);
    if (iter != mapbuffer_.end()) { // got id
      if (iter->second->can_read(length)) { // got data
        ret = iter->second->read(data, length);
        return ret;
      }
      //std::this_thread::yield();
    }
    
    if (!buffer_->can_read()) {
      // cout << "can not find data id:" << id <<  endl;
      continue;
    }
    string tmp_id;
    string tmp_data;
    ssize_t ret = buffer_->read(tmp_id, tmp_data); 

    if (tmp_id == id) 
    {
      if (tmp_data.size() != length) 
      {
        cout << "get data length:" << tmp_data.size() << " expected:" << length << endl;
      }
      memcpy(data, tmp_data.data(), tmp_data.size());
      return tmp_data.size();
    }

    if (mapbuffer_.find(tmp_id) == mapbuffer_.end()) {
      mapbuffer_[tmp_id] = make_shared<cycle_buffer>(1024 * 100);
    }

    // write the real data
    mapbuffer_[tmp_id]->write(tmp_data.data(), tmp_data.size());
  } while (true);

  return 0;
}
