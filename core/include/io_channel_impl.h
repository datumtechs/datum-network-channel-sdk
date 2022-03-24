#pragma once
#include "IChannel.h"
#include "config.h"
#include <atomic>
#include <condition_variable>
#include <iostream>
#include <iomanip>
#include <mutex>
#include <string>
#include <thread>
#include <vector>
using namespace std;

class IoChannelImpl {
private:
  IoChannelImpl() = default;
  IoChannelImpl(const IoChannelImpl&) = delete;
  IoChannelImpl(IoChannelImpl&&) = delete;
  IoChannelImpl& operator=(const IoChannelImpl&) = delete;
  IoChannelImpl& operator=(IoChannelImpl&&) = delete;
public:
  ~IoChannelImpl()
  {
    if(io_channel_)
    {
      delete io_channel_; 
      io_channel_=nullptr;
    }
  }
  static IoChannelImpl* Instance() 
  {
    static IoChannelImpl impl;
    return &impl;
  }

  string GetPolicy(const string &config_str);
  string Recv(const string& node_id, uint64_t recv_len);
  ssize_t Send(const string& node_id, const string& data);
  
  IChannel* CreateIoChannel(const string& self_node_id, const string &config_str, 
        error_callback error_cb=nullptr);

  // 等待服务器结束
  // void WaitServer(){if(server_) server_->wait();}
private:
  IChannel* io_channel_ = nullptr;
private:

  IChannel* CreateViaChannel(const NodeInfo& node_Info, shared_ptr<ChannelConfig> config,
      const set<ViaInfo>& remote_server_infos, const set<string>& clientNodeIds, 
      error_callback error_callback=nullptr);
};
