#include "net_io.h"
#include "sync_server.h"
#include "async_server.h"
#include "sync_client.h"
#include "async_client.h"
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
    map<string, shared_ptr<ClientConnection>>* ptr_client_conn_map)
{
#if ASYNC_SERVER
  // async server
  server_ = make_shared<AsyncServer>(server_info, ptr_client_conn_map);
  int thread_count = server_->get_thread_count();

  handle_threads_.resize(thread_count);
  for(int i = 0; i < thread_count; ++i)
  {
    handle_threads_[i] = thread(&BaseServer::Handle_Event, server_, i);
  }

  #if USE_CACHE
    #if MULTI_LOCKS
      int i = 0;
      handle_data_threads_.resize(ptr_client_conn_map->size());
      for(auto &v: *ptr_client_conn_map)
      {
        handle_data_threads_[i++] = thread(&BaseServer::Handle_Data, server_, v.first);
      }
    #else
      handle_data_threads_.resize(1);
      handle_data_threads_[0] = thread(&BaseServer::Handle_Data, server_);
    #endif
  #endif
  
#else
  // sync server
  server_ = make_shared<SyncServer>(server_info, ptr_client_conn_map);
#endif

  if(node_info_.address != node_info_.via_address)
  {
      ViaInfo via_info;
      via_info.address = node_info_.via_address;
    // #if(1==SSL_TYPE)  
    //   via_info.client_key_path_;
    //   via_info.client_cert_path_;
    // #elif(2==SSL_TYPE)  
    //   via_info.client_sign_key_path_;
    //   via_info.client_sign_cert_path_;
    //   via_info.client_enc_key_path_;
    //   via_info.client_enc_cert_path_;
    // #endif
      shared_ptr<grpc::ChannelCredentials> creds = grpc::InsecureChannelCredentials();
      cout << "server_info.address:" << server_info.address << endl;
      cout << "via_info.address:" << via_info.address << endl;
      auto channel = grpc::CreateChannel(via_info.address, creds);
      via_stub_ = VIAService::NewStub(channel);
      grpc::ClientContext context;
      // 添加注册到via的参数
      // context.AddMetadata("node_id", remote_nodeid);
      context.AddMetadata("task_id", taskid);
      context.AddMetadata("party_id", server_info.id);
      SignupReq reg_req;
      Boolean ret_code;

      reg_req.set_taskid(taskid);
      reg_req.set_partyid(server_info.id);
      reg_req.set_servicetype("");
      reg_req.set_address(server_info.address);

      via_stub_->Signup(&context, reg_req, &ret_code);
      
      if (false == ret_code.result()) 
      {
        string strErrMsg = "Signup via server failed!";
        cout << strErrMsg << endl;
        throw (strErrMsg);
      } 
      
      cout << "Signup via server succeed: " << via_info.address << endl;
  }
  
  cout << "server_info.id:" << server_info.id << ", via_address:" << server_info.via_address << endl;
  return true;
}

bool ViaNetIO::init(const string& taskid) 
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
    clients_thread_.push_back(std::thread(&AsyncClient::AsyncCompleteRpc, nid_to_server_map_[server_node_id]));
    gpr_log(GPR_INFO, "init async client connect, sids: %s.", via_server_infos_[i].id.c_str()); 
#else
    nid_to_server_map_[server_node_id] = make_shared<SyncClient>(via_server_infos_[i], taskid);
    gpr_log(GPR_INFO, "init sync client connect, sids: %s.", via_server_infos_[i].id.c_str());
#endif
  
  }

  gpr_log(GPR_INFO, "init all network connections succeed!"); 
  return true;
}

ssize_t ViaNetIO::recv(const string& remote_nodeid, const char* id, char* data,
      uint64_t length, int64_t timeout) 
{
  ssize_t ret = client_conn_map[remote_nodeid]->recv(id, data, length, timeout);
  gpr_log(GPR_DEBUG, "recv data from %s succeed, id:%s, size:%ld.", remote_nodeid.c_str(), id, ret);
  return ret;
}

ssize_t ViaNetIO::send(const string& remote_nodeid, const char* id, const char* data, 
      uint64_t length, int64_t timeout) 
{
  ssize_t ret = nid_to_server_map_[remote_nodeid]->send(node_info_.id, remote_nodeid, 
        id, data, length, timeout);
  gpr_log(GPR_DEBUG, "send data to %s succeed, id:%s, size:%ld.", remote_nodeid.c_str(), id, ret);
  return ret;
}
