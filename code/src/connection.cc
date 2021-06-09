// file connection.cc
#include "include/connection.h"

ClientConnection::ClientConnection(const string& self_nodeid, const string& server_addr)
{
	self_nodeid_ = self_nodeid;
	auto channel = grpc::CreateChannel(server_addr, grpc::InsecureChannelCredentials());
	stub_ = IoChannel::NewStub(channel);
}

// stream
/*
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

	std::unique_ptr<ClientWriter<SendRequest> > writer(
        stub_->Send(&context, &ret_code));

	int nLen = 0;
	int data_len = data.length();
	int one_len = 100000;
	if(0 == data_len % one_len)
	{
		nLen = data_len / one_len;
	}
	else
	{
		nLen = data_len / one_len + 1;
	}
	cout << "nLen:============" << endl;
	vector<string> vec_data(nLen);
	int nStart = 0;
	int nEnd = data_len;
	if(nLen)
	{
		nEnd = nLen;
	}
	string newData = data;
	for (int i = 0; i < nLen; i++) 
	{
		string sub_data = newData.substr(nStart, nEnd);
		req_info.set_data(sub_data);
		if (!writer->Write(req_info)) {
			cout << "Send message failed - ret code: " << ret_code.code() << endl;
			break;
		}
		nStart = nEnd;
		// 剩余数据长度
		data_len = data_len - nEnd;
		if(data_len < nEnd)
		{
			nEnd = data_len;
		}
		newData = newData.substr(nStart, data_len);
	}
	
	writer->WritesDone();
    grpc::Status status = writer->Finish();
    if (status.ok()) {
		cout << "Send message succeed - ret code: " << ret_code.code() << endl;
	} else {
		cout << "Send message failed - ret code: " << ret_code.code() << endl;
	}
	
	return 0;
}
*/

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
