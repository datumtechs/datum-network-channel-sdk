// file io_channel_server.h
#pragma once
#include <iostream>
#include <unordered_map>
#include <mutex>
#include<chrono>
#include<thread>
#include<map>
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

// 服务器
class IoChannelServer: public IoChannel::Service
{
public:
    bool close();
    bool wait();
    std::map<string, string>& get_data_map();
    IoChannelServer(const string& server_addr);
	Status Send(ServerContext* context, const SendRequest* request, RetCode* response);
    
private:
    std::mutex                    mtx_;
    std::unique_ptr<Server> server_;
    std::map<string, string> save_data_map_;
};
