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

#if ASYNC_SERVER
  server_ = make_shared<AsyncServer>(server_addr, ptr_client_conn_map);
  int enableCPUNum = sysconf(_SC_NPROCESSORS_ONLN);
  // 可用cpu核数-1
  int optimalUseCPUNum = enableCPUNum > 1 ? (enableCPUNum - 1): 1;
  // int optimalUseCPUNum = 1;
  handle_threads_.resize(optimalUseCPUNum);
  for(int i = 0; i < optimalUseCPUNum; ++i)
  {
    handle_threads_[i] = thread(&AsyncServer::Handle_Event, server_, i);
  }
#else
  server_ = make_shared<SyncServer>(server_addr, ptr_client_conn_map);
#endif
  
  return true;
}

bool ViaNetIO::init(const string& taskid) 
{
  // 初始化客户端连接过来的缓存
  uint32_t client_size = client_nodeids_.size();
  if(client_size > 0)
  {
    for (int i = 0; i < client_size; i++)
    {
      client_conn_map[client_nodeids_[i]] = make_shared<ClientConnection>();
    }

    // 启动服务
    StartServer(node_info_.address, &client_conn_map);
  }
  
  uint32_t nServerSize = via_server_infos_.size();
  conn_servers_.resize(nServerSize);
  for (int i = 0; i < nServerSize; i++) 
  {
#if ASYNC_CLIENT
    conn_servers_[i] = make_shared<AsyncClient>(via_server_infos_[i].address, taskid);
    clients_thread_.push_back(std::thread(&AsyncClient::AsyncCompleteRpc, conn_servers_[i]));
    cout << "init async connect, sids:" << via_server_infos_[i].id << endl;
#else
    conn_servers_[i] = make_shared<SyncClient>(via_server_infos_[i].address, taskid);
    cout << "init sync connect, sids:" << via_server_infos_[i].id << endl;
#endif
    string server_node_id =  via_server_infos_[i].id;
    nid_to_server_map_[server_node_id] = conn_servers_[i];
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

  ssize_t ret = nid_to_server_map_[remote_nodeid]->send(node_info_.id, remote_nodeid, 
        id, data, length, timeout);
  return ret;
}
