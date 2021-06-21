// file connection.cc
#include "connection.h"
#include <chrono>   
using namespace chrono;
ClientConnection::ClientConnection(const string& server_addr, const string& taskid)
{
	task_id_ = taskid;

	auto channel = grpc::CreateChannel(server_addr, grpc::InsecureChannelCredentials());
	stub_ = IoChannel::NewStub(channel);
	cout << "server_addr:" << server_addr << endl;
}

// stream
ssize_t ClientConnection::send(const string& self_nodeid, const string& remote_nodeid, 
	const string& task_id, const string& msg_id, const string& data, int64_t timeout)
{
	// cout << "ClientConnection::send" << endl;
	
	SendRequest req_info;
	// 发送客户端的nodeid到服务器
	req_info.set_nodeid(self_nodeid);
	req_info.set_id(msg_id);
	req_info.set_data(data);

	RetCode ret_code;

	auto start_time = system_clock::now();
	auto end_time   = start_time;
	while(true)
	{
		grpc::ClientContext context;
		// 添加注册到via的参数
		// context.AddMetadata("node_id", remote_nodeid);
		context.AddMetadata("task_id", task_id);
		context.AddMetadata("party_id", remote_nodeid);

		Status status = stub_->Send(&context, req_info, &ret_code);
		if (status.ok()) 
		{
			break;
		} 
		else 
		{
			end_time = system_clock::now();
			auto duration = duration_cast<microseconds>(end_time - start_time);
			auto cost_time = double(duration.count()) * 
				microseconds::period::num / microseconds::period::den;

			cout << "send data to " << remote_nodeid << " failed, cost " 
				 << cost_time << " seconds." << endl;
			if(cost_time >= timeout)
				break;
			sleep(1);
		}
	}
	
	cout << "Send message() - ret code: " << ret_code.code() << endl;
	
	return data.length();
}

