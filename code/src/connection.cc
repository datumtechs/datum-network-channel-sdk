// file connection.cc
#include "include/connection.h"

ClientConnection::ClientConnection(const string& server_addr)
{
	auto channel = grpc::CreateChannel(server_addr, grpc::InsecureChannelCredentials());
	stub_ = IoChannel::NewStub(channel);

	// thread_ = std::thread(&ClientConnection::AsyncCompleteRpc, this);
	// thread_.join();
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
	// req_info.set_data(data);
	req_info.set_timeout(timeout);
	grpc::ClientContext context;
	RetCode ret_code;

	std::unique_ptr<ClientWriter<SendRequest>> writer(
        stub_->Send(&context, &ret_code));

	// 切分大数据
	cout << "start to split========" << endl;
	int nLen = 0;
	int data_len = data.length();
	int one_len = 100;
	if(0 == data_len % one_len)
	{
		nLen = data_len / one_len;
	}
	else
	{
		nLen = data_len / one_len + 1;
	}
	cout << "nLen:============" << endl;
	int nStartIndex = 0;
	int nEndIndex = one_len;
	if(1 == nLen)
	{
		nEndIndex = data_len;
	}
	string newData = data;
	for (int i = 0; i < (nLen-1); i++) 
	{
		string sub_data = newData.substr(nStartIndex, nEndIndex);
		req_info.set_data(sub_data);
		if (!writer->Write(req_info)) 
		{
			cout << "Send message failed - ret code: " << ret_code.code() << endl;
			break;
		}
		nStartIndex = nStartIndex + one_len;
	}

	if((nStartIndex+one_len)>data_len)
	{
		nEndIndex = data_len - nStartIndex;
	}

	req_info.set_data(newData.substr(nStartIndex, nEndIndex));
	if (!writer->Write(req_info)) 
	{
		cout << "Send message failed - ret code: " << ret_code.code() << endl;
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

ssize_t ClientConnection::send(const string& self_nodeid, const string& remote_nodeid, 
	const string& task_id, const string& msg_id, const string& data, int64_t timeout)
{
	cout << "ClientConnection::send" << endl;
	SendRequest req_info;
	// 发送客户端的nodeid到服务器
	req_info.set_nodeid(self_nodeid);
	req_info.set_id(msg_id);
	req_info.set_data(data);
	req_info.set_timeout(timeout);

	/*
	// Call object to store rpc data
    AsyncClientCall* call = new AsyncClientCall;

    // stub_->PrepareAsyncSayHello() creates an RPC object, returning
    // an instance to store in "call" but does not actually start the RPC
    // Because we are using the asynchronous API, we need to hold on to
    // the "call" instance in order to get updates on the ongoing RPC.
    call->response_reader =
        stub_->PrepareAsyncSend(&call->context, req_info, &cq_);

    // StartCall initiates the RPC call
    call->response_reader->StartCall();

    // Request that, upon completion of the RPC, "reply" be updated with the
    // server's response; "status" with the indication of whether the operation
    // was successful. Tag the request with the memory address of the call
    // object.
    call->response_reader->Finish(&call->reply, &call->status, (void*)call);
	*/

	grpc::ClientContext context;
	// 添加注册到via的参数
    // context.AddMetadata("node_id", remote_nodeid);
	context.AddMetadata("task_id", task_id);
	context.AddMetadata("party_id", remote_nodeid);
	
	RetCode ret_code;
	stub_->Send(&context, req_info, &ret_code);
	
	cout << "Send message() - ret code: " << ret_code.code() << endl;
	
	return data.length();
}

/*
// Loop while listening for completed responses.
// Prints out the response from the server.
bool ClientConnection::AsyncCompleteRpc() 
{
    void* got_tag;
    bool ok = false;
    // Block until the next result is available in the completion queue "cq".
    while (cq_.Next(&got_tag, &ok))
	{
      // The tag in this example is the memory location of the call object
      AsyncClientCall* call = static_cast<AsyncClientCall*>(got_tag);

      // Verify that the request was completed successfully. Note that "ok"
      // corresponds solely to the request for updates introduced by Finish().
      GPR_ASSERT(ok);

      if (call->status.ok())
        std::cout << "MyServer received: " << to_string(call->reply.code()) << std::endl;
      else
        std::cout << "RPC failed" << std::endl;

      // Once we're complete, deallocate the call object.
      delete call;
    }

    cout << "call AsyncCompleteRpc() over==========" << endl;
	return true;
}
*/

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
