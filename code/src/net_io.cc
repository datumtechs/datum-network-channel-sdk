#include "include/net_io.h"
#include "include/io_channel.h"
#include <set>

void BasicIO::close() {}

BasicIO::~BasicIO(){
    close();
}

BasicIO::BasicIO(const NodeInfo &node_id, const vector<ViaInfo>& server_infos,
    map<string, string>* share_data_map, error_callback error_callback)
  : node_info_(node_id), via_server_infos_(server_infos), share_data_map_(share_data_map),
    handler(error_callback) {}


bool BasicIO::init() 
{
  // init_inner();
  // 创建多线程
  vector<thread> client_threads(via_server_infos_.size());
  vector<shared_ptr<ClientConnection>> clietns(via_server_infos_.size());
  auto conn_f = [&](const ViaInfo &info, int i) -> bool {
    // // 创建io通道
    cout << "create io channel, sids:" << info.id << endl;
    clietns[i] = make_shared<ClientConnection>(node_info_.id, 
        info.address);
    return true;
  };

  for (int i = 0; i < via_server_infos_.size(); i++) {
    client_threads[i] = thread(conn_f, via_server_infos_[i], i);
  }

  for (int i = 0; i < via_server_infos_.size(); i++) {
    client_threads[i].join();
  }

  for (int i = 0; i < via_server_infos_.size(); i++)
  {
    string ser_node_id =  via_server_infos_[i].id;
    // 保存到connection_map
    cout << "save io channel sids:" << via_server_infos_[i].id << endl;
    connection_map[ser_node_id] = clietns[i];
  }
  
  cout << "init all network connections succeed!" << endl;
  return true;
}

ssize_t BasicIO::recv(const string& node_id, string& data, const string& id, int64_t timeout) 
{
  mutex mtx_;
  lock_guard<mutex> guard(mtx_);
  string strSaveId = node_id + ":" + id;
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
  return 0;
}

ssize_t BasicIO::send(const string& node_id, const string& data, const string& id, int64_t timeout) 
{
  cout << "BasicIO::send, node_id: " << node_id << ", id" << id << endl;
  ssize_t ret = connection_map[node_id]->send(id, data, timeout);
  return ret;
}
