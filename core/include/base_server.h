
// file base_server.h
#pragma once
#include "config.h"
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
using grpc::ServerBuilder;
using namespace std;

#define RET_SUCCEED_CODE 200

class BaseServer
{
public:
	BaseServer(const NodeInfo& server_info, 
		map<string, shared_ptr<ClientConnection>>* ptr_client_conn_map);
	virtual ~BaseServer(){};
	virtual bool close(){if(base_server_) base_server_->Shutdown();return true;}

#if ASYNC_SERVER
	virtual void Handle_Event(const int numEvent) = 0;
	virtual int get_thread_count() = 0;
#if USE_CACHE
	#if MULTI_LOCKS
		virtual void Handle_Data(const string& nodeid) = 0;
	#else 
		virtual void Handle_Data() = 0;
	#endif
#endif
#endif

protected:
	std::unique_ptr<Server> base_server_;
	std::shared_ptr<ServerBuilder> builder_ = nullptr;
	map<string, shared_ptr<ClientConnection>>* ptr_client_conn_map_ = nullptr;
};

