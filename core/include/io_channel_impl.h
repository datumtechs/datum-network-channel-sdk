#pragma once
#include "IChannel.h"
#include "src/io_channel_server.cc"
// #include "src/io_channel_async_server.cc"
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
  ~IoChannelImpl(){CloseServer();}

  bool CloseServer() { if(server_) server_->close(); return true;}

  bool StartServer(const string& server_addr);
  shared_ptr<IChannel> CreateChannel(const string& node_id, const string &config_str, 
      const bool& is_start_server=false, error_callback error_cb=nullptr);

  // 等待服务器结束
  void WaitServer(){if(server_) server_->wait();}
private:
  shared_ptr<BasicIO> CreateViaChannel(const NodeInfo& node_idInfo, 
      const vector<ViaInfo>& serverInfos, map<string, string>* share_data_map_=nullptr, 
      error_callback error_callback=nullptr);

private:
  shared_ptr<IoChannelServer> server_ = nullptr;
  // shared_ptr<IoChannelAsyncServer> server_ = nullptr;
};


class GRpcChannel : public IChannel {
public:
    GRpcChannel(shared_ptr<BasicIO> net_io) {_net_io = net_io;}
    ~GRpcChannel(){}
    virtual void SetErrorCallback(error_callback error_cb) {}
    virtual int64_t Recv(const string& node_id, const string& id, string& data, int64_t timeout = -1);
    virtual int64_t Send(const string& node_id, const string& id, const string& data, int64_t timeout = -1);

    string GetCurrentVia();
    string GetCurrentAddress();
    string GetTaskId();

    virtual vector<string> GetDataNodeIDs();
    virtual map<string, int> GetComputationNodeIDs();
    virtual vector<string> GetResultNodeIDs();
    virtual string GetCurrentNodeID();
    virtual vector<string> GetConnectedNodeIDs();
private:
    shared_ptr<BasicIO> _net_io = nullptr;
};
