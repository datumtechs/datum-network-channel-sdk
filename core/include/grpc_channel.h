#pragma once
#include "IChannel.h"
#include "config.h"
#include "net_io.h"
using namespace std;

class GRpcChannel : public IChannel {
public:
    GRpcChannel(){}

    GRpcChannel(shared_ptr<BasicIO> net_io, shared_ptr<ChannelConfig> config, const NodeInfo& node_info):
      _net_io(net_io), channel_config_(config), self_node_info_(node_info) {}
    ~GRpcChannel();
    virtual void SetErrorCallback(error_callback error_cb) {}
    virtual void Flush() {}
    virtual int64_t Recv(const char* node_id, const char* id, char* data, uint64_t length, int64_t timeout=-1);
    virtual int64_t Send(const char* node_id, const char* id, const char* data, uint64_t length, int64_t timeout=-1);

    virtual const NodeIDVec* GetDataNodeIDs();
    virtual const NodeIDMap* GetComputationNodeIDs();
    virtual const NodeIDVec* GetResultNodeIDs();
    virtual const char* GetCurrentNodeID();
    virtual const NodeIDVec* GetConnectedNodeIDs();
    void SetConnectedNodeIDs(const vector<string>& connected_nodes) { connected_nodes_ = connected_nodes; }
private:
    const vector<string>& getDataNodeIDs();
    const map<string, int>& getComputationNodeIDs();
    const vector<string>& getResultNodeIDs();
    const string& getCurrentNodeID();
    const vector<string>& getConnectedNodeIDs();

    void DestroyCurrentNode();
    void DestroyDataNodes();
    void DestroyComputationNodes();
    void DestroyResultNodes();
    void DestroyConnectedNodes();
private:
    shared_ptr<BasicIO> _net_io = nullptr;
    shared_ptr<ChannelConfig> channel_config_ = nullptr;
    NodeInfo self_node_info_;
    vector<string> connected_nodes_;
};
