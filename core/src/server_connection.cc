// file server_connection.cc
#include "server_connection.h"
#include "simple_buffer.h"
#include <thread>
#include <chrono>   
using namespace chrono;
ServerConnection::ServerConnection(const string& server_addr, const string& taskid)
{
	task_id_ = taskid;
	auto channel = grpc::CreateChannel(server_addr, grpc::InsecureChannelCredentials());
	stub_ = IoChannel::NewStub(channel);
	// cout << "create channel, server_addr:" << server_addr << endl;
}

ssize_t ServerConnection::send(const string& self_nodeid, const string& remote_nodeid, 
	const string& task_id, const string& msg_id, const char* data, const size_t nLen, int64_t timeout)
{
	// cout << "ServerConnection::send, remote_nodeid:" << remote_nodeid << ", msg_id:" << msg_id
  //   << ", data length:" << nLen << endl;
	
  SendRequest req_info;
  // 发送客户端的nodeid到服务器
  req_info.set_nodeid(self_nodeid);
  // req_info.set_id(msg_id);
  // req_info.set_data(data, nLen);
  simple_buffer buffer(msg_id, data, nLen);
  req_info.set_data((const char*)buffer.data(), buffer.len());
  
  RetCode ret_code;

  auto start_time = system_clock::now();
  auto end_time   = start_time;
  
  do {
    grpc::ClientContext context;
    // 添加注册到via的参数
    // context.AddMetadata("node_id", remote_nodeid);
    context.AddMetadata("task_id", task_id);
    context.AddMetadata("party_id", remote_nodeid);

    // cout << "ClientConnection stub_->Send1111, remote_nodeid:" << remote_nodeid << endl;
    Status status = stub_->Send(&context, req_info, &ret_code);
    // cout << "ClientConnection stub_->Send2222, remote_nodeid:" << remote_nodeid << endl;
    if (status.ok()) 
    {
      // cout << "send data to " << remote_nodeid << " succeed=====" << endl;
      break;
    } 
    else 
    {
      end_time = system_clock::now();
      auto duration = duration_cast<microseconds>(end_time - start_time);
      auto cost_time = double(duration.count()) * 
        microseconds::period::num / microseconds::period::den;

      // cout << "send data to " << remote_nodeid << " failed, cost " 
      //   << cost_time << " seconds." << endl;
      if(cost_time >= timeout)
        break;
      std::this_thread::yield();
      // this_thread::sleep_for(chrono::milliseconds(SLEEP_TIME_MILLI_SECONDS));
    }
  } while(true);
	
	// cout << "Send data succeed==============" << endl;
	return nLen;
}
