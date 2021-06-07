#include "include/io_channel_impl.h"
#include "include/net_io.h"
#include "include/common.h"
#include <set>

bool IoChannelImpl::StartServer(const string& server_addr)
{
  auto start_f = [&](const string& _addr) -> bool {
    // // 创建io通道
    server_ = make_shared<IoChannelServer>(_addr);
    return true;
  };
  
  thread server_thread = thread(start_f, server_addr);
  server_thread.join();
  return true;
}

shared_ptr<BasicIO> IoChannelImpl::CreateViaChannel(const NodeInfo& node_idInfo, 
      const vector<ViaInfo>& serverInfos, map<string, string>* share_data_map_, 
      error_callback error_callback) 
{      
  shared_ptr<BasicIO> net_io =  nullptr;
  net_io = make_shared<ViaNetIO>(node_idInfo, serverInfos, share_data_map_, error_callback);
  if (net_io->init()) 
  {
    // return std::dynamic_pointer_cast<IChannel>(make_shared<GRpcChannel>(net_io));
    return net_io;
  }
 
  error_callback(node_idInfo.id, "", -1, "init io failed!", (void*)"user_data");
  return nullptr;
}

shared_ptr<BasicIO> IoChannelImpl::CreateChannel(const string& node_id, const string &config_str, 
      const bool& is_start_server, const string& server_addr, error_callback error_cb) 
{
  // 启动服务器
  if(is_start_server)
  {
    StartServer(server_addr);
  }
  shared_ptr<ChannelConfig> config = make_shared<ChannelConfig>(config_str);
  NodeInfo node_info;
  vector<ViaInfo> serverInfos;
  // 根据nodeid获取数据节点或计算节点或接收结果节点信息
  const Node& node = config->GetNode(node_id);
  vector<NODE_TYPE> node_types = config->GetNodeType(node_id);
  // 获取节点信息
  CopyNodeInfo(node_info, node);
  set<string> nodeid_set;

  if(isNodeType(node_types, NODE_TYPE_DATA) || isNodeType(node_types, NODE_TYPE_COMPUTE))
  {
    // 遍历计算节点
    for (int i = 0; i < config->compute_config_.P.size(); i++) 
    {
      // 获取计算节点的nodeid
      string nid = config->compute_config_.P[i].NODE_ID;
      string via = config->nodeid_to_via_[nid];
      if (node_id != nid && nodeid_set.find(nid) == nodeid_set.end()) 
      {
        ViaInfo viaTmp;
        viaTmp.id = nid;
        viaTmp.via = via;
        viaTmp.address = config->via_to_info_[via];
        cout << "id: " << nid << ", via: " << viaTmp.via << ", address: " << viaTmp.address << endl;
        serverInfos.push_back(viaTmp);
        // 保存除自身外的计算节点
        nodeid_set.insert(nid);
      }
    }
  }
  
  if(isNodeType(node_types, NODE_TYPE_COMPUTE))
  {
    cout << "is compute node!" << endl;
    // 遍历结果接收节点
    for (int i = 0; i < config->result_config_.P.size(); i++) 
    {
      cout << "handle compute node" << endl;
      string nid = config->result_config_.P[i].NODE_ID;
      string via = config->nodeid_to_via_[nid];
      if (node_id != nid && nodeid_set.find(nid) == nodeid_set.end()) 
      {
        ViaInfo viaTmp;
        viaTmp.id = nid;
        // 节点所在via
        viaTmp.via = via;
        // via信息
        viaTmp.address = config->via_to_info_[viaTmp.via];
        cout << "id: " << nid << ", via: " << viaTmp.via << ", address: " << viaTmp.address << endl;
        serverInfos.push_back(viaTmp);
        nodeid_set.insert(nid);
      }
    }  
  }

  string strNodeInfo = node_info.address + ":" + to_string(node_info.port) + ", id:" + node_info.id;
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
  // 服务器模型, 设置共享内存
  map<string, string>* share_data_map_ = nullptr;
  if(server_)
  {
    share_data_map_ = &(server_->get_data_map());
  }
  return CreateViaChannel(node_info, serverInfos, share_data_map_, error_cb);
}

ssize_t GRpcChannel::Recv(const string& node_id, const string& id, string& data, int64_t timeout) {
  return _net_io->recv(node_id, data, get_binary_string(id), timeout); 
  // return _net_io->recv(node_id, data, id, timeout); 
}

ssize_t GRpcChannel::Send(const string& node_id, const string& id, const string& data, int64_t timeout) {
  // cout << "GRPC send, id====" << id << endl;
  return _net_io->send(node_id, data, get_binary_string(id), timeout);
}

