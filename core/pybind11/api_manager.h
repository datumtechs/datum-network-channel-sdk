#pragma once
#include "io_channel_impl.h"
using namespace std;

class APIManager 
{
public:
  APIManager() {}
  shared_ptr<IChannel> create_channel(const string& node_id, const std::string &config_str, 
      error_callback error_cb) 
  {
    return IoChannelImpl::Instance()->CreateIoChannel(node_id, config_str, error_cb);
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
