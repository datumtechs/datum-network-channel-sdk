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

  string recv_msg(const string& node_id, const string& msg_id, uint64_t msg_len, uint64_t timeout=0);
  ssize_t send_msg(const string& node_id, const string& msg_id, const string& data, 
        uint64_t msg_len, uint64_t timeout=0);
  
  IChannel* CreateIoChannel(const string& node_id, const string &config_str, 
        error_callback error_cb=nullptr);

  // 等待服务器结束
  // void WaitServer(){if(server_) server_->wait();}
public:
  IChannel* io_channel_ = nullptr;
private:

  IChannel* CreateViaChannel(const NodeInfo& node_idInfo, shared_ptr<ChannelConfig> config,
      const vector<ViaInfo>& serverInfos, const vector<string>& clientNodeIds, 
      error_callback error_callback=nullptr);
};
