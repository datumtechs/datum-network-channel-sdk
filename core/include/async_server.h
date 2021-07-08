
// file async_server.h
#pragma once
#include "client_connection.h"
#include <unistd.h>
#include <functional>
#include <stdexcept>
#include <memory>
#include <iostream>
#include <cmath>
#include <map>
#include "assert.h"
#include <grpc++/grpc++.h>
#include "io_channel.grpc.pb.h"


using grpc::Server;
using grpc::ServerAsyncResponseWriter;
using grpc::ServerAsyncWriter;
using grpc::ServerAsyncReader;
using grpc::ServerAsyncReaderWriter;
using grpc::ServerBuilder;
using grpc::ServerContext;
using grpc::ServerCompletionQueue;
using grpc::CompletionQueue;
using grpc::Status;
using io_channel::IoChannel;
using io_channel::SendRequest;
using io_channel::RetCode;
using namespace std;

#define RET_SUCCEED_CODE 200

class CommonCallData
{
public:
	explicit CommonCallData(IoChannel::AsyncService* service, ServerCompletionQueue* cq):
						service_(service), cq_(cq),status_(CREATE){}

	virtual ~CommonCallData(){/*std::cout << "CommonCallData destructor" << std::endl;*/}

	virtual void Proceed(map<string, shared_ptr<ClientConnection>>* ptr_client_conn_map = nullptr) = 0;
public:
    // The means of communication with the gRPC runtime for an asynchronous
    // server.
    IoChannel::AsyncService* service_;
    // The producer-consumer queue where for asynchronous server notifications.
    ServerCompletionQueue* cq_;
    // Context for the rpc, allowing to tweak aspects of it such as the use
    // of compression, authentication, as well as to send metadata back to the
    // client.
    ServerContext ctx_;
    // What we get from the client.
    SendRequest request_;
    // What we send back to the client.
    RetCode reply_;
	// Let's implement a tiny state machine with the following states.
    enum CallStatus { CREATE, PROCESS, FINISH };
    CallStatus status_;  // The current serving state.
	std::mutex mtx_;
};

class CallData: public CommonCallData
{
public:
	using CommonCallData::CommonCallData;
	CallData(IoChannel::AsyncService* service, ServerCompletionQueue* cq):
		CommonCallData(service, cq), responder_(&ctx_){Proceed();}
	virtual void Proceed(map<string, shared_ptr<ClientConnection>>* ptr_client_conn_map = nullptr);
private:
    ServerAsyncResponseWriter<RetCode> responder_;
};

class AsyncServer
{
public:
	AsyncServer(const string& server_address, 
		map<string, shared_ptr<ClientConnection>>* ptr_client_conn_map);
	~AsyncServer();
	bool close();
	void Handle_Event(const int numEvent);

private:
	int enableCPUNum_ = 0;
	int optimalUseCPUNum_ = 0;
	map<int, std::unique_ptr<ServerCompletionQueue>> map_cq_;
	IoChannel::AsyncService service_;
	std::unique_ptr<Server> server_;
	std::shared_ptr<ServerBuilder> builder_ = nullptr;
	map<string, shared_ptr<ClientConnection>>* ptr_client_conn_map_ = nullptr;
};

