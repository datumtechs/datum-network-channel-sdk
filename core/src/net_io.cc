#include "net_io.h"
#include "sync_server.h"
#include "async_server.h"
#include "sync_client.h"
#include "IChannel.h"
#include <unistd.h>
#include <set>

BasicIO::BasicIO(const NodeInfo &node_info, const set<ViaInfo>& remote_server_infos,
    const set<string>& client_nodeids, error_callback error_callback )
  : node_info_(node_info), remote_server_infos_(remote_server_infos), client_nodeids_(client_nodeids),
    handler(error_callback) {}

bool ViaNetIO::StartServer(const string& taskid, const NodeInfo& server_info,
    MapClientConn* ptr_client_conn_map)
{
#if ASYNC_SERVER
  cout << "start async server, address:" << server_info.address << endl;
  // async server
  server_ = make_shared<AsyncServer>(server_info, taskid, ptr_client_conn_map);
#else
  string address = "address:" + server_info.address;
  if(server_info.address.empty()) {
    address = "public ip:" + server_info.public_ip_;
  }
  cout << "start sync server, " << address
       << ", Glacier2 info: {" << server_info.glacier2_info.GetInfo() << "}" 
       << ", IceGrid info: {" << server_info.ice_grid_info.GetInfo() << "}" << endl;
  // sync server
  server_ = make_shared<SyncServer>(server_info, taskid, ptr_client_conn_map);
#endif

  return true;
}

bool ViaNetIO::init(const shared_ptr<ChannelConfig> config)
{
  const string taskid = config->task_id_;
  // 初始化客户端连接过来的缓存
  uint32_t client_size = client_nodeids_.size();
  if(client_size > 0)
  {
    for(auto& iter: client_nodeids_) {
      string client_nodeid = iter;
      client_conn_map_[client_nodeid] = make_shared<ClientConnection>(client_nodeid);
    #if USE_BUFFER
      client_conn_map_[client_nodeid]->SetBufferSize(config->buffer_size_); 
    #endif
      client_conn_map_[client_nodeid]->SetRecvTimeOut(config->send_timeout_*1000);
    }

    // 启动服务
    StartServer(taskid, node_info_, &client_conn_map_);
  }
  for(auto& iter: remote_server_infos_) {
    string remote_node_id = iter.id;
    client_obj_map_[remote_node_id] = make_shared<SyncClient>(iter, taskid);
    client_obj_map_[remote_node_id]->SetSendTimeOut(config->send_timeout_*1000);
    client_obj_map_[remote_node_id]->CheckConnStatus(config->node_id_, config->conn_timeout_*1000, config->ping_time_*1000000);
  }
  
  return true;
}

ssize_t ViaNetIO::recv(const string& remote_nodeid, const char* id, char* data,
      uint64_t length, int64_t timeout) 
{
  if(client_conn_map_.find(remote_nodeid) == client_conn_map_.end()) {
    cout << "recv data:\nnot find remote nodeid:" << remote_nodeid << endl;
    HANDLE_EXCEPTION_EVENT(C_EVENT_CODE_NO_FIND_NID, "", remote_nodeid.c_str());
  }
  ssize_t ret = client_conn_map_[remote_nodeid]->recv(id, data, length, timeout);
  return ret;
}

ssize_t ViaNetIO::send(const string& remote_nodeid, const char* id, const char* data, 
      uint64_t length, int64_t timeout) 
{
  if(client_obj_map_.find(remote_nodeid) == client_obj_map_.end()) {
    cout << "send data:\nnot find remote nodeid:" << remote_nodeid << endl;
    HANDLE_EXCEPTION_EVENT(C_EVENT_CODE_NO_FIND_NID, "", remote_nodeid.c_str());
  }

  ssize_t ret = client_obj_map_[remote_nodeid]->send(node_info_.id, remote_nodeid, 
        id, data, length, timeout);    
  return ret;
}
