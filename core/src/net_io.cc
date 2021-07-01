#include "net_io.h"
#include "IChannel.h"
#include <unistd.h>
#include <set>
#include <chrono>   
using namespace chrono;

void BasicIO::close() {}

BasicIO::~BasicIO(){close();}

BasicIO::BasicIO(const NodeInfo &node_info, const vector<ViaInfo>& server_infos,
    const vector<string>& client_nodeids,
    error_callback error_callback )
  : node_info_(node_info), via_server_infos_(server_infos), client_nodeids_(client_nodeids),
    handler(error_callback) {}

bool ViaNetIO::StartServer(const string& server_addr, 
    map<string, shared_ptr<ClientConnection>>* ptr_client_conn_map)
{
  server_ = make_shared<SyncServer>(server_addr, ptr_client_conn_map);
  // server_ = make_shared<IoChannelAsyncServer>(server_addr);
  return true;
}

bool ViaNetIO::init(const string& taskid) 
{
  // 初始化客户端连接过来的缓存
  int32_t client_size = client_nodeids_.size();
  if(client_size > 0)
  {
    vector<shared_ptr<ClientConnection>> clients_conn(client_nodeids_.size());
    for (int i = 0; i < client_nodeids_.size(); i++) 
    {
      client_conn_map[client_nodeids_[i]] = make_shared<ClientConnection>();
    }

    // 启动服务
    StartServer(node_info_.address, &client_conn_map);
  }
 
  // 初始化连接服务器的连接
  vector<shared_ptr<SyncClient>> conn_servers(via_server_infos_.size());
  for (int i = 0; i < via_server_infos_.size(); i++) 
  {
    conn_servers[i] = make_shared<SyncClient>(via_server_infos_[i].address, taskid);
  }
  /*
  // 创建多线程
  vector<thread> client_threads(via_server_infos_.size());
  auto conn_f = [&](const ViaInfo &info, int i) -> bool {
    // 创建io通道
    cout << "create io channel, sids:" << info.id << endl;
    clients[i] = make_shared<ClientConnection>(info.address, taskid);
    return true;
  };

  for (int i = 0; i < via_server_infos_.size(); i++) {
    client_threads[i] = thread(conn_f, via_server_infos_[i], i);
  }

  for (int i = 0; i < via_server_infos_.size(); i++) {
    client_threads[i].join();
  }
  */

  for (int i = 0; i < via_server_infos_.size(); i++)
  {
    string ser_node_id =  via_server_infos_[i].id;
    // 保存到connection_map
    cout << "save io channel sids:" << via_server_infos_[i].id << endl;
    sync_client_map[ser_node_id] = conn_servers[i];
  }
  
  cout << "init all network connections succeed!" << endl;
  return true;
}

ssize_t ViaNetIO::recv(const string& remote_nodeid, const char* id, char* data,
      uint64_t length, int64_t timeout) 
{
  ssize_t ret = client_conn_map[remote_nodeid]->recv(id, data, length, timeout);
  return ret;
}

ssize_t ViaNetIO::send(const string& remote_nodeid, const char* id, const char* data, 
      uint64_t length, int64_t timeout) 
{
  // cout << "ViaNetIO::send, remote node_id: " << remote_nodeid << ", task id: " 
  //      << sync_client_map[remote_nodeid]->task_id_ << ", id: " << id << endl;

  ssize_t ret = sync_client_map[remote_nodeid]->send(node_info_.id, remote_nodeid, 
      sync_client_map[remote_nodeid]->task_id_, id, data, length, timeout);
  return ret;
}
