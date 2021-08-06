// file async_client.cc
#include "async_client.h"
#if USE_BUFFER_
#include "simple_buffer.h"
#endif
#include <thread>
#include <chrono>   
using namespace chrono;

AsyncClientCall::AsyncClientCall(const string& task_id, const string& remote_nodeid,
		const SendRequest& request, CompletionQueue& cq_, 
		std::unique_ptr<IoChannel::Stub>& stub_):AbstractAsyncClientCall()
{
	request_ = request;
	remote_nodeid_ = remote_nodeid;
	// std::cout << "[Proceed11]: new client, name:" << request.name() << std::endl;
	context.AddMetadata("task_id", task_id);
    context.AddMetadata("party_id", remote_nodeid);
	
	responder = stub_->PrepareAsyncSend(&context, request, &cq_);
	responder->StartCall();
	responder->Finish(&reply, &status, (void*)this);
	callStatus = PROCESS ;
}

AsyncClient::AsyncClient(const string& server_addr, const string& taskid,
		const char* server_cert, const char* client_key, const char* client_cert)
{
	task_id_ = taskid;
  	shared_ptr<grpc::ChannelCredentials> creds;
#ifdef SSL_TYPE
	if(0 == SSL_TYPE)
	{
		creds = grpc::InsecureChannelCredentials();
	}
	else if(1 == SSL_TYPE)
	{
		if(nullptr == server_cert || nullptr == client_key || nullptr == client_cert)
		{
			cerr << "Invalid client certificate, please check!" << endl;
			return;
		}
		grpc::SslCredentialsOptions ssl_opts;
		ssl_opts.pem_root_certs  = server_cert;
		ssl_opts.pem_private_key = client_key;
		ssl_opts.pem_cert_chain  = client_cert;
		creds = grpc::SslCredentials(ssl_opts);
	}
	else if(2 == SSL_TYPE)
	{

	}
#endif
	auto channel = grpc::CreateChannel(server_addr, creds);
	stub_ = IoChannel::NewStub(channel);
}

void AsyncClient::SendReqAgain(const AbstractAsyncClientCall* call)
{
	new AsyncClientCall(task_id_, call->remote_nodeid_, call->request_, cq_, stub_);
}

// Assembles the client's payload and sends it to the server.
ssize_t AsyncClient::send(const string& self_nodeid, const string& remote_nodeid, 
	  	const string& msg_id, const char* data, const size_t nLen, int64_t timeout)
{
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

	AsyncClientCall* call = new AsyncClientCall(task_id_, remote_nodeid, req_info, cq_, stub_);
	
	return nLen;
}

void AsyncClient::AsyncCompleteRpc()
{
	void* got_tag;
	bool ok = false;
	while(cq_.Next(&got_tag, &ok))
	{
		AbstractAsyncClientCall* call = static_cast<AbstractAsyncClientCall*>(got_tag);
		if(call->callStatus == call->PROCESS)
		{
			GPR_ASSERT(ok);
			
			if (!call->status.ok())
			{
				// 重发
				SendReqAgain(call);
			}

			// 完成后，释放call对象
			delete call;
		}
	}
}
