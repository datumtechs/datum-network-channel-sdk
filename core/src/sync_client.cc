// file sync_client.cc
#include "sync_client.h"

#if USE_BUFFER
#include "simple_buffer.h"
#endif
#include <thread>
#include <chrono>   
using namespace chrono;

SyncClient::SyncClient(const ViaInfo& via_info, const string& taskid):
  BaseClient(via_info, taskid){}

// SyncClient::SyncClient(const NodeInfo& node_info, const string& taskid):
//   BaseClient(node_info, taskid){}

ssize_t SyncClient::send(const string& self_nodeid, const string& remote_nodeid, 
    const string& msg_id, const char* data, const size_t nLen, int64_t timeout)
{	
  // SendRequest req_info;
  // 发送客户端的nodeid到服务器
  // req_info.nodeid = self_nodeid;
  // req_info.id = msg_id;
  // req_info.data = data;
  auto start_time = system_clock::now();
  auto end_time   = start_time;
  int64_t elapsed = 0;
  system_clock::time_point deadline = start_time + std::chrono::milliseconds(timeout);
  
  Ice::ByteSeq vec_send_data;
  vec_send_data.resize(nLen);
  memcpy(&vec_send_data[0], data, nLen);

  do {
    int status = 1;
    auto time = std::chrono::steady_clock::now().time_since_epoch().count();
    // cout << "客户端开始发送时间戳:" << time << endl;
    try{
      Ice::Context context;
      status = stub_->send(self_nodeid, msg_id, vec_send_data, context);
    } catch (const Ice::Exception& ex) {
        // cerr << ex << endl;
        sleep(1);
        status = 1;
    }
    
    if (0 == status) 
    {
      break;
    } 
    else 
    {
      end_time = system_clock::now();
      elapsed = duration_cast<duration<int64_t, std::milli>>(end_time - start_time).count();

      if(elapsed >= timeout)
      {
        return 0;
      }
    }
  } while(true);

	return nLen;
}
