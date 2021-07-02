
// file async_client.h
#pragma once
#include <string>
#include <functional>
#include <stdexcept>
#include <memory>
#include <iostream>
#include <cmath>
#include "assert.h"
#include "unistd.h"

#include <grpc++/grpc++.h>
#include <thread>

#include "io_channel.grpc.pb.h"
using namespace std;

using grpc::Channel;
using grpc::ClientAsyncResponseReader;
using grpc::ClientContext;
using grpc::ClientAsyncReader;
using grpc::ClientAsyncWriter;
using grpc::ClientAsyncReaderWriter;
using grpc::CompletionQueue;
using grpc::Status;
using io_channel::IoChannel;
using io_channel::SendRequest;
using io_channel::RetCode;


class AbstractAsyncClientCall
{
public:
	enum CallStatus { PROCESS, FINISH, DESTROY };

	explicit AbstractAsyncClientCall():callStatus(PROCESS){}
	virtual ~AbstractAsyncClientCall(){}
	RetCode reply;
	ClientContext context;
	Status status;
	CallStatus callStatus ;
	SendRequest request_;
	string remote_nodeid_;
	virtual uint32_t Proceed(bool = true) = 0;
};

class AsyncClientCall: public AbstractAsyncClientCall
{
	std::unique_ptr< ClientAsyncResponseReader<RetCode> > responder;
public:
	AsyncClientCall(const string& task_id, const string& remote_nodeid, 
		const SendRequest& request, CompletionQueue& cq_, 
		std::unique_ptr<IoChannel::Stub>& stub_);
	virtual uint32_t Proceed(bool ok = true){};
};

class AsyncClient
{
public:
    AsyncClient(const string& server_addr, const string& taskid);

	void SendReqAgain(const AbstractAsyncClientCall* call);
	// Assembles the client's payload and sends it to the server.

	ssize_t send(const string& self_nodeid, const string& remote_nodeid, 
	  	const string& msg_id, const char* data, const size_t nLen, int64_t timeout = -1L);


	~AsyncClient(){cq_.Shutdown();}
	void AsyncCompleteRpc();
public:
	string task_id_;
private:
    // Out of the passed in Channel comes the stub, stored here, our view of the
    // server's exposed services.
    std::unique_ptr<IoChannel::Stub> stub_;

    // The producer-consumer queue we use to communicate asynchronously with the
    // gRPC runtime.
    CompletionQueue cq_;
	bool is_already_sync = false; 
};

