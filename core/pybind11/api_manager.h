#pragma once
#include "io_channel_impl.h"
using namespace std;

void process_error(const char* current_node_id, const char* node_id, int errorno, 
    const char* errormsg, void*user_data) 
{
  cout << "The connection to party " << node_id << " is broken, errorno:" << errorno 
       << " errormsg:" << errormsg << " user_data:" << user_data << endl;
}

class APIManager 
{
public:
  APIManager() {}
  IChannel* create_channel(const string& node_id, const std::string &config_str) 
  {
    return IoChannelImpl::Instance()->CreateIoChannel(node_id, config_str, process_error);
  }

  string recv_msg(const string& node_id, const string& msg_id, uint64_t msg_len, uint64_t timeout)
  {
    return IoChannelImpl::Instance()->recv_msg(node_id, msg_id, msg_len, timeout);
  }

  ssize_t send_msg(const string& node_id, const string& msg_id, const string& data, uint64_t msg_len,
      uint64_t timeout)
  {
    return IoChannelImpl::Instance()->send_msg(node_id, msg_id, data, msg_len, timeout);
  }

};
