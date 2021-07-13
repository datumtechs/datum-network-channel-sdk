// file sync_client.cc
#include "sync_client.h"
#if USE_BUFFER_
#include "simple_buffer.h"
#endif
#include <thread>
#include <chrono>   
using namespace chrono;
SyncClient::SyncClient(const string& server_addr, const string& taskid)
{
	task_id_ = taskid;
	auto channel = grpc::CreateChannel(server_addr, grpc::InsecureChannelCredentials());
	stub_ = IoChannel::NewStub(channel);
	// cout << "create channel, server_addr:" << server_addr << endl;
}

ssize_t SyncClient::send(const string& self_nodeid, const string& remote_nodeid, 
    const string& msg_id, const char* data, const size_t nLen, int64_t timeout)
{
	// cout << "SyncClient::send, remote_nodeid:" << remote_nodeid << ", msg_id:" << msg_id
  //   << ", data length:" << nLen << endl;
	
  SendRequest req_info;
  // 发送客户端的nodeid到服务器
  req_info.set_nodeid(self_nodeid);

#if USE_BUFFER_
  simple_buffer buffer(msg_id, data, nLen);
  req_info.set_data((const char*)buffer.data(), buffer.len());
#else
  req_info.set_id(msg_id);
  req_info.set_data(data, nLen);
#endif
  
  RetCode ret_code;

  auto start_time = system_clock::now();
  auto end_time   = start_time;
  int64_t elapsed = 0;
  system_clock::time_point deadline = start_time + std::chrono::milliseconds(timeout);
  do {
    grpc::ClientContext context;
    // 添加注册到via的参数
    // context.AddMetadata("node_id", remote_nodeid);
    context.AddMetadata("task_id", task_id_);
    context.AddMetadata("party_id", remote_nodeid);

    // 设置阻塞等待和超时时间
    context.set_wait_for_ready(true);
    context.set_deadline(deadline);
    Status status = stub_->Send(&context, req_info, &ret_code);
    if (status.ok()) 
    {
      // cout << "send data to " << remote_nodeid << " succeed=====" << endl;
      break;
    } 
    else 
    {
      end_time = system_clock::now();
      elapsed = duration_cast<duration<int64_t, std::milli>>(end_time - start_time).count();

      // cout << "send data to " << remote_nodeid << " failed, cost " 
      //   << elapsed << " milliseconds." << endl;
      if(elapsed >= timeout)
      {
          cout << "send request timeout!" << endl;
          return 0;
      }
    }
  } while(true);
	
	return nLen;
}
