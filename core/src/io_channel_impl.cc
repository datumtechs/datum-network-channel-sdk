#include "io_channel_impl.h"
#include "tcp_channel.h"
#include <set>

string IoChannelImpl::Recv(const string& node_id, uint64_t recv_len) 
{
  string str(recv_len, 0);
  if(!io_channel_){cout << "io channel is nullptr." << endl; return "";}
  io_channel_->Recv(node_id.c_str(), "", &str[0], recv_len, 0);
  return str;
}

ssize_t IoChannelImpl::Send(const string& node_id, const string& data)
{
  if(!io_channel_){cout << "io channel is nullptr." << endl; return 0;}
  return io_channel_->Send(node_id.c_str(), "", data.c_str(), data.size(), 0);
}

IChannel* IoChannelImpl::CreateViaChannel(const NodeInfo& node_info, 
      shared_ptr<ChannelConfig> config, const vector<ViaInfo>& serverInfos, 
      const vector<string>& clientNodeIds, error_callback error_callback) 
{
  shared_ptr<BasicIO> net_io =  nullptr;
  net_io = make_shared<ViaNetIO>(node_info, serverInfos, clientNodeIds, error_callback);
  if (net_io->init(config))
  { 
    TcpChannel* tcp_channel = new TcpChannel(net_io, config, node_info);
    tcp_channel->SetConnectedNodeIDs(clientNodeIds);
    io_channel_ = tcp_channel;

    return io_channel_;
  }
 
  error_callback(node_info.id.c_str(), "", -1, "init io failed!", (void*)"user_data");
  return nullptr;
}

IChannel* IoChannelImpl::CreateIoChannel(const string& node_id, const string &config_str, 
        error_callback error_cb) 
{
  shared_ptr<ChannelConfig> config = make_shared<ChannelConfig>(config_str);
  NodeInfo node_info;
  vector<ViaInfo> serverInfos;
  // 根据nodeid获取数据节点或计算节点或接收结果节点信息
  const Node& node = config->GetNode(node_id);
  // 获取节点信息
  config->CopyNodeInfo(node_info, node);

  // 获取本节点对应的via地址
  string via_name = config->nodeid_to_via_[node_info.id];
  node_info.via_address = config->via_to_address_[via_name];
  // 获取本节点对应的glacier2地址
  string glacier2_name = config->nodeid_to_glacier2_[node_info.id];
  node_info.glacier2_info = config->glacier2_to_info_[glacier2_name];

  // 获取本节点对应的IceGrid地址
  string ice_grid_name = config->nodeid_to_icegrid_[node_info.id];
  node_info.ice_grid_info = config->icegrid_to_info_[ice_grid_name];
  /*
  if("" == node_info.via_address)
  {
    string strErrMsg = "The service node " + node_info.id + " does not have a VIA address!";
    cout << strErrMsg << endl;
    throw (strErrMsg);
  }
      
  if("" == node.ADDRESS)
  {
    string strErrMsg = "The address corresponding to the " + node_info.id + " node server is empty!";
    cout << strErrMsg << endl;
    throw (strErrMsg);
  }
  */
    
  vector<string> clientNodeIds; 
  config->GetNodeInfos(clientNodeIds, serverInfos, node_id);

  return CreateViaChannel(node_info, config, serverInfos, clientNodeIds, error_cb);
}
