#include "io_channel_impl.h"
#include "tcp_channel.h"
#include <set>

string IoChannelImpl::Recv(const string& remote_node_id, uint64_t recv_len) 
{
  string str(recv_len, 0);
  if(!io_channel_){cout << "io channel is nullptr." << endl; return "";}
  io_channel_->Recv(remote_node_id.c_str(), "", &str[0], recv_len, 0);
  return str;
}

ssize_t IoChannelImpl::Send(const string& remote_node_id, const string& data)
{
  if(!io_channel_){cout << "io channel is nullptr." << endl; return 0;}
  return io_channel_->Send(remote_node_id.c_str(), "", data.c_str(), data.size(), 0);
}

IChannel* IoChannelImpl::CreateViaChannel(const NodeInfo& node_info, 
      shared_ptr<ChannelConfig> config, const set<ViaInfo>& remote_server_infos, 
      const set<string>& clientNodeIds, error_callback error_callback) 
{
  shared_ptr<BasicIO> net_io =  nullptr;
  net_io = make_shared<ViaNetIO>(node_info, remote_server_infos, clientNodeIds, error_callback);
  if (net_io->init(config))
  { 
    TcpChannel* tcp_channel = new TcpChannel(net_io, config, node_info);
    vector<string> clientNIds;
    for(auto& iter:clientNodeIds) {
      clientNIds.push_back(iter);
    }
    tcp_channel->SetConnectedNodeIDs(clientNIds);
    io_channel_ = tcp_channel;

    return io_channel_;
  }
 
  error_callback(node_info.id.c_str(), "", -1, "init io failed!", (void*)"user_data");
  return nullptr;
}

string IoChannelImpl::GetPolicy(const string &config_str)
{
  shared_ptr<ChannelConfig> config = make_shared<ChannelConfig>(config_str);
  string strPolicy = "";
  for(auto& nid: config->task_nodes_) {
    if(!strPolicy.empty()) {strPolicy += ",";}
    // cout << "task nodeid:" << nid << endl;
    string node_id = nid;
    strPolicy += "\"" + node_id + "\":";
    set<string> clientNodeIds;
    set<ViaInfo> serverInfos;
    config->GetNodeInfos(clientNodeIds, serverInfos, node_id);

    string strSid = "";
    for(auto& iter: serverInfos) {
      if(!strSid.empty()) {strSid += ",";}
      strSid += "\"" + iter.id + "\"";
    }
    strSid = "[" + strSid + "]";
    strPolicy += strSid;
  }

  strPolicy = "{" + strPolicy + "}";
  return strPolicy;
}

IChannel* IoChannelImpl::CreateIoChannel(const string& self_node_id, const string &config_str, 
        error_callback error_cb) 
{
  shared_ptr<ChannelConfig> config = make_shared<ChannelConfig>(config_str);
  NodeInfo node_info;
  set<ViaInfo> remote_server_infos;
  // 根据nodeid获取数据节点或计算节点或接收结果节点信息
  const Node& node = config->GetNode(self_node_id);
  // 获取节点信息
  config->CopyNodeInfo(node_info, node);

  set<string> clientNodeIds;
  config->GetNodeInfos(clientNodeIds, remote_server_infos, self_node_id);
  return CreateViaChannel(node_info, config, remote_server_infos, clientNodeIds, error_cb);
}
