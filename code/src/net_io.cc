#include "include/net_io.h"
#include "include/IChannel.h"
#include <set>

void BasicIO::close() {}

BasicIO::~BasicIO(){close();}

BasicIO::BasicIO(const NodeInfo &node_info, const vector<ViaInfo>& server_infos,
    map<string, string>* share_data_map, error_callback error_callback)
  : node_info_(node_info), via_server_infos_(server_infos), 
    share_data_map_(share_data_map), handler(error_callback) {}

bool ViaNetIO::init() 
{
  // 创建多线程
  vector<thread> client_threads(via_server_infos_.size());
  vector<shared_ptr<ClientConnection>> clients(via_server_infos_.size());
  
  auto conn_f = [&](const ViaInfo &info, int i) -> bool {
    // // 创建io通道
    cout << "create io channel, sids:" << info.id << endl;
    clients[i] = make_shared<ClientConnection>(info.address);
    return true;
  };

  for (int i = 0; i < via_server_infos_.size(); i++) {
    
    client_threads[i] = thread(conn_f, via_server_infos_[i], i);

    /*
    clients[i] = make_shared<ClientConnection>(via_server_infos_[i].address);

    client_threads[i] = thread(&ClientConnection::AsyncCompleteRpc, clients[i]);
    */
  }

  for (int i = 0; i < via_server_infos_.size(); i++) {
    client_threads[i].join();
  }

  for (int i = 0; i < via_server_infos_.size(); i++)
  {
    string ser_node_id =  via_server_infos_[i].id;
    // 保存到connection_map
    cout << "save io channel sids:" << via_server_infos_[i].id << endl;
    connection_map[ser_node_id] = clients[i];
  }
  
  cout << "init all network connections succeed!" << endl;
  return true;
}

ssize_t ViaNetIO::recv(const string& remote_nodeid, string& data, const string& id, int64_t timeout) 
{
  mutex mtx_;
  lock_guard<mutex> guard(mtx_);
  string strSaveId = remote_nodeid + ":" + id;
  cout << "recv strSaveId:" << strSaveId << endl;
  cout << "share_data_map_->size == " << share_data_map_->size() << endl;
  // 从本地缓存中获取
  data = "";
  if(share_data_map_->find(strSaveId) != share_data_map_->end())
  {
    data = (*share_data_map_)[strSaveId];
    // cout << "recv data = " << data << endl;
    // 删除数据
    share_data_map_->erase(strSaveId);
  }
  return data.length();
}

ssize_t ViaNetIO::send(const string& remote_nodeid, const string& data, const string& id, int64_t timeout) 
{
  cout << "ViaNetIO::send, remote node_id: " << remote_nodeid << ", task id: " 
       << channel_config_->task_id_ << ", id: " << id << endl;
  
  ssize_t ret = connection_map[remote_nodeid]->send(node_info_.id, remote_nodeid, 
      channel_config_->task_id_, id, data, timeout);
  return ret;
}
