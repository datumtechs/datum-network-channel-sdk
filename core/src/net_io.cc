#include "net_io.h"
#include <fstream>
#include <sstream>
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


static string get_file_contents(const string& fpath)
{
    ifstream ifile(fpath);
    if(!ifile.good())
    {
        cout << "file is not exist:" << fpath << endl;
        return "";
    }
    ostringstream buf;
    char ch;
    while(buf&&ifile.get(ch))
    buf.put(ch);
    return buf.str();
}


bool ViaNetIO::StartServer(const NodeInfo& server_info,
    map<string, shared_ptr<ClientConnection>>* ptr_client_conn_map)
{
  char* ptr_root_cert = nullptr;
  char* ptr_server_key = nullptr;
  char* ptr_server_cert = nullptr;

#if USE_SSL
  auto root_crt = get_file_contents(server_info.ca_cert_path_); // for verifying clients
  auto server_key = get_file_contents(server_info.server_key_path_);
  auto server_cert = get_file_contents(server_info.server_cert_path_);

  ptr_root_cert = const_cast<char*>(root_crt.c_str());
  ptr_server_key = const_cast<char*>(server_key.c_str());
  ptr_server_cert = const_cast<char*>(server_cert.c_str());
#endif 

#if ASYNC_SERVER
  // async server
  server_ = make_shared<AsyncServer>(server_info.address, ptr_client_conn_map, 
      ptr_root_cert, ptr_server_key, ptr_server_cert);
  
  int thread_count = server_->get_thread_count();

  handle_threads_.resize(thread_count);
  for(int i = 0; i < thread_count; ++i)
  {
    handle_threads_[i] = thread(&AsyncServer::Handle_Event, server_, i);
  }

  #if USE_CACHE
    #if MULTI_LOCKS
      int i = 0;
      handle_data_threads_.resize(ptr_client_conn_map->size());
      for(auto &v: *ptr_client_conn_map)
      {
        handle_data_threads_[i++] = thread(&AsyncServer::Handle_Data, server_, v.first);
      }
    #else 
      handle_data_threads_.resize(1);
      handle_data_threads_[0] = thread(&AsyncServer::Handle_Data, server_);
    #endif
  #endif
  
#else
  // sync server
  server_ = make_shared<SyncServer>(server_info.address, ptr_client_conn_map,
      ptr_root_cert, ptr_server_key, ptr_server_cert);
  
#endif
  
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
    StartServer(node_info_, &client_conn_map);
  }
  
  uint32_t nServerSize = via_server_infos_.size();
  conn_servers_.resize(nServerSize);
  for (int i = 0; i < nServerSize; i++) 
  {
    char* ptr_server_cert = nullptr;
    char* ptr_client_key = nullptr;
    char* ptr_client_cert = nullptr;

#if USE_SSL
    auto server_cert = get_file_contents(via_server_infos_[i].server_cert_path_);
    auto client_key  = get_file_contents(via_server_infos_[i].client_key_path_);
    auto client_cert = get_file_contents(via_server_infos_[i].client_cert_path_);
    ptr_server_cert = const_cast<char*>(server_cert.c_str());
    ptr_client_key = const_cast<char*>(client_key.c_str());
    ptr_client_cert = const_cast<char*>(client_cert.c_str());
#endif 

#if ASYNC_CLIENT
    conn_servers_[i] = make_shared<AsyncClient>(via_server_infos_[i].address, taskid,
        ptr_server_cert, ptr_client_key, ptr_client_cert);
    clients_thread_.push_back(std::thread(&AsyncClient::AsyncCompleteRpc, conn_servers_[i]));

    cout << "init async connect, sids:" << via_server_infos_[i].id << endl;
#else
    conn_servers_[i] = make_shared<SyncClient>(via_server_infos_[i].address, taskid,
        ptr_server_cert, ptr_client_key, ptr_client_cert);

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
