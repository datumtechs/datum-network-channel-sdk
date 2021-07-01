// file async_server.cc
#include "async_server.h"
#include "simple_buffer.h"
#include <thread>
#include <chrono>   
using namespace chrono;

void CallData::Proceed(map<string, shared_ptr<ClientConnection>>* ptr_client_conn_map)
{
	if (status_ == CREATE)
	{
		status_ = PROCESS;
		service_->RequestSend(&ctx_, &request_, &responder_, cq_, cq_, this);
	}
	else if (status_ == PROCESS)
	{
		new CallData(service_, cq_);
		status_ = FINISH;
		// 返回值
		reply_.set_code(RET_SUCCEED_CODE);
		responder_.Finish(reply_, Status::OK, this);
		// 保存数据
		std::unique_lock<mutex> guard(buffer_mtx_);
		const string& nodeId = request_.nodeid();
		// cout << "nodeid:===" << nodeId << endl;
		auto iter = ptr_client_conn_map->find(nodeId);
		if(iter == ptr_client_conn_map->end())
		{
			return;
		}
		// The msgid is already included in the data  
		const string& data = request_.data();
		iter->second->buffer_->write(data.data(), data.size());
	}
	else
	{
		GPR_ASSERT(status_ == FINISH);
		delete this;
	}
}

bool AsyncServer::close()
{
    // 关闭服务
    server_->Shutdown();
	// Always shutdown the completion queue after the server.
	cq_->Shutdown();
	return true;
}

AsyncServer::~AsyncServer()
{
	close();
}

AsyncServer::AsyncServer(const string& server_address, 
	map<string, shared_ptr<ClientConnection>>* ptr_client_conn_map)
{
	ptr_client_conn_map_ = ptr_client_conn_map;
	// ServerBuilder builder;
	builder_ = make_shared<ServerBuilder>();
	// Listen on the given address without any authentication mechanism.
	builder_->AddListeningPort(server_address, grpc::InsecureServerCredentials());
	// Register "service_" as the instance through which we'll communicate with
	// clients. In this case it corresponds to an *asynchronous* service.
	builder_->RegisterService(&service_);
	// Get hold of the completion queue used for the asynchronous communication
	// with the gRPC runtime.
	cq_ = builder_->AddCompletionQueue();
	// Finally assemble the server.
	server_ = builder_->BuildAndStart();
	cout << "Server listening on " << server_address << endl;

	// Proceed to the server's main loop.
	// Spawn a new CallData instance to serve new clients.
	new CallData(&service_, cq_.get());
}

void AsyncServer::Handle_Event()
{
	void* tag;  // uniquely identifies a request.
    bool ok;
	while (true) 
	{
      GPR_ASSERT(cq_->Next(&tag, &ok));
      GPR_ASSERT(ok);
      static_cast<CallData*>(tag)->Proceed(ptr_client_conn_map_);
    }
}
