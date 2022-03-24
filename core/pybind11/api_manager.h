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

  string GetPolicy(const std::string &config_str)
  {
    return IoChannelImpl::Instance()->GetPolicy(config_str);
  }

  IChannel* create_channel(const string& node_id, const std::string &config_str) 
  {
    return IoChannelImpl::Instance()->CreateIoChannel(node_id, config_str, process_error);
  }

  string Recv(const string& node_id, uint64_t recv_len)
  {
    return IoChannelImpl::Instance()->Recv(node_id, recv_len);
  }

  ssize_t Send(const string& node_id, const string& data)
  {
    return IoChannelImpl::Instance()->Send(node_id, data);
  }
};
