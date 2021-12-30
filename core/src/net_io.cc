#include "net_io.h"
#include "sync_server.h"
#include "async_server.h"
#include "sync_client.h"
#include "IChannel.h"
#include <unistd.h>
#include <set>
#include <chrono>   
using namespace chrono;

BasicIO::BasicIO(const NodeInfo &node_info, const vector<ViaInfo>& server_infos,
    const vector<string>& client_nodeids, error_callback error_callback )
  : node_info_(node_info), via_server_infos_(server_infos), client_nodeids_(client_nodeids),
    handler(error_callback) {}

bool ViaNetIO::StartServer(const string& taskid, const NodeInfo& server_info,
    unordered_map<string, shared_ptr<ClientConnection>>* ptr_client_conn_map)
{
#if ASYNC_SERVER
  cout << "start async server, address:" << server_info.address << endl;
  // async server
  server_ = make_shared<AsyncServer>(server_info, ptr_client_conn_map);
#else
  cout << "start sync server, address:" << server_info.address << endl;
  // sync server
  server_ = make_shared<SyncServer>(server_info, ptr_client_conn_map);
#endif

  return true;
}

bool ViaNetIO::init(const string& taskid, const useconds_t usec) 
{
  // 初始化客户端连接过来的缓存
  uint32_t client_size = client_nodeids_.size();
  if(client_size > 0)
  {
    string client_nodeid = "";

    for (int i = 0; i < client_size; i++)
    {
      client_nodeid = client_nodeids_[i];
      client_conn_map[client_nodeid] = make_shared<ClientConnection>(client_nodeid);
    }

    // 启动服务
    StartServer(taskid, node_info_, &client_conn_map);
  }
  
  uint32_t nServerSize = via_server_infos_.size();
  for (int i = 0; i < nServerSize; i++) 
  {
    string server_node_id =  via_server_infos_[i].id;
#if ASYNC_CLIENT
    nid_to_server_map_[server_node_id]  = make_shared<AsyncClient>(via_server_infos_[i], taskid);
    clients_thread_.emplace_back(std::thread(&AsyncClient::AsyncCompleteRpc, nid_to_server_map_[server_node_id]));
    // gpr_log(GPR_INFO, "init async client connect, sids: %s.", via_server_infos_[i].id.c_str()); 
#else
    nid_to_server_map_[server_node_id] = make_shared<SyncClient>(via_server_infos_[i], taskid);
    nid_to_server_map_[server_node_id]->CheckConnect(usec);
    // gpr_log(GPR_INFO, "init sync client connect, sids: %s.", via_server_infos_[i].id.c_str());
#endif
  
  }

  return true;
}

ssize_t ViaNetIO::recv(const string& remote_nodeid, const char* id, char* data,
      uint64_t length, int64_t timeout) 
{
  ssize_t ret = client_conn_map[remote_nodeid]->recv(id, data, length, timeout);
  // gpr_log(GPR_DEBUG, "recv data from %s succeed, id:%s, size:%ld.", remote_nodeid.c_str(), id, ret);
  return ret;
}

ssize_t ViaNetIO::send(const string& remote_nodeid, const char* id, const char* data, 
      uint64_t length, int64_t timeout) 
{
  ssize_t ret = nid_to_server_map_[remote_nodeid]->send(node_info_.id, remote_nodeid, 
        id, data, length, timeout);
  // gpr_log(GPR_DEBUG, "send data to %s succeed, id:%s, size:%ld.", remote_nodeid.c_str(), id, ret);
  return ret;
}
