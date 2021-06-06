// file connection.cc
#include "include/connection.h"

ClientConnection::ClientConnection(const string& self_nodeid, const string& server_addr)
{
	self_nodeid_ = self_nodeid;
	auto channel = grpc::CreateChannel(server_addr, grpc::InsecureChannelCredentials());
	stub_ = IoChannel::NewStub(channel);
}

ssize_t ClientConnection::send(const string& msg_id, const string& data, int64_t timeout)
{
	cout << "ClientConnection::send" << endl;
	SendRequest req_info;
	// 发送客户端的nodeid到服务器
	req_info.set_nodeid(self_nodeid_);
	req_info.set_id(msg_id);
	req_info.set_data(data);
	req_info.set_timeout(timeout);
	grpc::ClientContext context;
	RetCode ret_code;
	stub_->Send(&context, req_info, &ret_code);
	
	cout << "Send message() - ret code: " << ret_code.code() << endl;
	
	return 0;
}

/*
ssize_t ClientConnection::recv(const string& msg_id, string& data, int64_t timeout = -1L)
{
	RecvRequest recv_req;
	recv_req.set_id(msg_id);
	recv_req.set_timeout(timeout);
	
	grpc::ClientContext context;
	RetData ret_data;
	stub_->Recv(&context, recv_req, &ret_data);
	
	data = ret_data.data();
	return 0;
}
*/


