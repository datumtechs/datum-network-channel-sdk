
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

/*
四种使用缓存组合方案：
1.单线程/单把锁/单个缓存队列;
2.单线程/多把锁/多个缓存队列;
3.多线程/单把锁/单个缓存队列;
4.多线程/多把锁/多个缓存队列;

四种不使用缓存组合方案：
1.单线程/单把锁/无;
2.单线程/多把锁/无;
3.多线程/单把锁/无;
4.多线程/多把锁/无;
*/

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

	virtual void Proceed(void* ptr_save = nullptr, void* ptr_mtx = nullptr, void* ptr_cv = nullptr) = 0;

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
};

class CallData: public CommonCallData
{
public:
	using CommonCallData::CommonCallData;
	CallData(IoChannel::AsyncService* service, ServerCompletionQueue* cq):
		CommonCallData(service, cq), responder_(&ctx_){Proceed();}

	virtual void Proceed(void* ptr_save = nullptr, void* ptr_mtx = nullptr, void* ptr_cv = nullptr);

private:
    ServerAsyncResponseWriter<RetCode> responder_;
};

class AsyncServer
{
public:
	AsyncServer(const string& server_address, 
		map<string, shared_ptr<ClientConnection>>* ptr_client_conn_map,
		const char* root_crt = nullptr, const char* server_key = nullptr, const char* server_cert = nullptr);
	~AsyncServer();
	bool close();
	void Handle_Event(const int numEvent);
	int get_thread_count() {return thread_count_;}
#if USE_CACHE
	#if MULTI_LOCKS
		void Handle_Data(const string& nodeid);
	#else 
		void Handle_Data();
	#endif
#endif

private:
#if MULTI_LOCKS
	map<string, shared_ptr<mutex>> map_mtx_;
	#if USE_CACHE
		map<string, shared_ptr<queue<SendRequest>>> map_send_queue_;
		map<string, shared_ptr<condition_variable>> map_cv_;
	#endif
#else
	mutex mtx_;
	#if USE_CACHE
		queue<SendRequest> send_queue_;
		condition_variable cv_;
	#endif
#endif

	int thread_count_ = 0;
	map<int, std::unique_ptr<ServerCompletionQueue>> map_cq_;
	IoChannel::AsyncService service_;
	std::unique_ptr<Server> server_;
	std::shared_ptr<ServerBuilder> builder_ = nullptr;
	map<string, shared_ptr<ClientConnection>>* ptr_client_conn_map_ = nullptr;
};

