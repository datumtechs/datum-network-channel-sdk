#pragma once
#include "IChannel.h"
#include "config.h"
#include "net_io.h"
#include <atomic>
#include <condition_variable>
#include <iostream>
#include <iomanip>
#include <map>
#include <mutex>
#include <string>
#include <thread>
#include <vector>
using namespace std;

class IoChannelImpl {
public:
  IoChannelImpl(){}
  ~IoChannelImpl(){}
  
  shared_ptr<IChannel> CreateIoChannel(const string& node_id, const string &config_str, 
        error_callback error_cb=nullptr);

  // 等待服务器结束
  // void WaitServer(){if(server_) server_->wait();}
private:

  shared_ptr<IChannel> CreateViaChannel(const NodeInfo& node_idInfo, shared_ptr<ChannelConfig> config,
      const vector<ViaInfo>& serverInfos, const vector<string>& clientNodeIds, 
      error_callback error_callback=nullptr);
};


class GRpcChannel : public IChannel {
public:
    GRpcChannel(){}
    GRpcChannel(shared_ptr<BasicIO> net_io, shared_ptr<ChannelConfig> config, const NodeInfo& node_info):
      _net_io(net_io), channel_config_(config), self_node_info_(node_info) {}
    ~GRpcChannel();
    virtual void SetErrorCallback(error_callback error_cb) {}
    virtual int64_t Recv(const char* node_id, const char* id, char* data, uint64_t length, int64_t timeout=-1);
    virtual int64_t Send(const char* node_id, const char* id, const char* data, uint64_t length, int64_t timeout=-1);

    // virtual const char* GetCurrentVia();
    // virtual const char* GetCurrentAddress();
    // virtual const char* GetTaskId();
    
    virtual const char* GetDataNodeIDs();
    virtual const char* GetComputationNodeIDs();
    virtual const char* GetResultNodeIDs();
    virtual const char* GetCurrentNodeID();
    virtual const char* GetConnectedNodeIDs();
private:
    shared_ptr<BasicIO> _net_io = nullptr;
    shared_ptr<ChannelConfig> channel_config_ = nullptr;
    NodeInfo self_node_info_;

    const char* p_node_id_ = nullptr;
    const char* p_data_nodes_ = nullptr;
    const char* p_computation_nodes_ = nullptr;
    const char* p_result_nodes_ = nullptr;
    const char* p_connected_nodes_ = nullptr;
};
