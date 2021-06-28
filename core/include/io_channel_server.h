// file io_channel_server.h
#pragma once
#include "client_connection.h"
#include <iostream>
#include <unordered_map>
#include <mutex>
#include<chrono>
#include<thread>
#include<map>
#include<queue>

using namespace std;

#include "io_channel.grpc.pb.h"
#include <grpc/grpc.h>
#include <grpc++/server.h>
#include <grpc++/server_builder.h>
#include <grpc++/server_context.h>
#include <grpc++/security/server_credentials.h>
using io_channel::IoChannel;
using io_channel::SendRequest;
using io_channel::RetCode;

using grpc::Server;
using grpc::ServerBuilder;
using grpc::ServerContext;
using grpc::ServerReader;
using grpc::ServerReaderWriter;
using grpc::ServerWriter;
using grpc::Status;

#define SLEEP_TIME_MILLI_SECONDS 500
#define RET_SUCCEED_CODE 200

// 服务器
class IoChannelServer: public IoChannel::Service
{
public:
    bool close();
    bool wait();

    IoChannelServer(const string& server_addr, 
        map<string, shared_ptr<ClientConnection>>* ptr_client_conn_map);
	Status Send(ServerContext* context, const SendRequest* request, RetCode* response);
    
private:
    std::mutex                    mtx_;
    std::unique_ptr<Server> server_;
    map<string, shared_ptr<ClientConnection>>* ptr_client_conn_map_ = nullptr;
};
