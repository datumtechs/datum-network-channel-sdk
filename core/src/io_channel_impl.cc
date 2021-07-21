#include "io_channel_impl.h"
#include "grpc_channel.h"
#include <set>

string IoChannelImpl::recv_msg(const string& node_id, const string& msg_id, uint64_t msg_len, 
        uint64_t timeout) 
{
  string str(msg_len, 0);
  if(timeout <= 0)
    timeout = 10 * 1000000;
  if(!io_channel_){cout << "io channel is nullptr." << endl; return "";}
  io_channel_->Recv(node_id.c_str(), msg_id.c_str(), &str[0], msg_len, timeout);
  return str;
}

ssize_t IoChannelImpl::send_msg(const string& node_id, const string& msg_id, const string& data, 
        uint64_t msg_len, uint64_t timeout)
{
  if(msg_len > data.size())
        msg_len = data.size();
  if(timeout <= 0)
    timeout = 10 * 1000000;
  if(!io_channel_){cout << "io channel is nullptr." << endl; return 0;}
  return io_channel_->Send(node_id.c_str(), msg_id.c_str(), data.c_str(), msg_len, timeout);
}

shared_ptr<IChannel> IoChannelImpl::CreateViaChannel(const NodeInfo& node_info, 
      shared_ptr<ChannelConfig> config, const vector<ViaInfo>& serverInfos, 
      const vector<string>& clientNodeIds, error_callback error_callback) 
{
  shared_ptr<BasicIO> net_io =  nullptr;
  net_io = make_shared<ViaNetIO>(node_info, serverInfos, clientNodeIds, error_callback);
  if (net_io->init(config->task_id_)) 
  {
    shared_ptr<GRpcChannel> grpc_channel = make_shared<GRpcChannel>(net_io, config, node_info);
    return std::dynamic_pointer_cast<IChannel>(grpc_channel);
  }
 
  error_callback(node_info.id.c_str(), "", -1, "init io failed!", (void*)"user_data");
  return nullptr;
}

shared_ptr<IChannel> IoChannelImpl::CreateIoChannel(const string& node_id, const string &config_str, 
        error_callback error_cb) 
{
  shared_ptr<ChannelConfig> config = make_shared<ChannelConfig>(config_str);
  NodeInfo node_info;
  vector<ViaInfo> serverInfos;
  // 根据nodeid获取数据节点或计算节点或接收结果节点信息
  const Node& node = config->GetNode(node_id);

  vector<NODE_TYPE> node_types = config->GetNodeType(node_id);
  // 获取节点信息
  config->CopyNodeInfo(node_info, node);

  // 获取本节点对应的via地址
  string via_name = config->nodeid_to_via_[node_info.id];
  node_info.via_address = config->via_to_address_[via_name];

  // 启动服务器
  vector<string> clientNodeIds;
 
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
    
  // cout << "start server, node.ADDRESS: " << node.ADDRESS << endl;

  config->GetNodeInfos(clientNodeIds, serverInfos, node_id);
  /*
  string strNodeInfo = "address: " + node_info.address + ", id:" + node_info.id;
  cout << "node_info=========:" << strNodeInfo << endl;
  string strServerInfo = "";
  string nodeid = "nodeid: ";
  string via = "via: ";
  string address = "address: ";
  for(int i=0; i < serverInfos.size(); ++i)
  {
      ViaInfo tmp = serverInfos[i];
      nodeid = nodeid + " " + tmp.id;
      via = via + " " + tmp.via;
      address = address + " \n" + tmp.address;
  }
  strServerInfo = nodeid + ", " + via + ", " + address;
  cout << "server info=========:" << strServerInfo << endl;
  */

  io_channel_ = CreateViaChannel(node_info, config, serverInfos, clientNodeIds, error_cb);
  return io_channel_;
}
