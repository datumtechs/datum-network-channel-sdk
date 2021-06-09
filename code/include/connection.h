// file connection.h
#pragma once

#include "protos/io_channel.grpc.pb.h"
#include "src/io_channel_server.cc"
#include <unordered_map>
#include <mutex>
// #include <grpc++/server.h>
// #include <grpc++/server_builder.h>
// #include <grpc++/server_context.h>
#include <grpc/grpc.h>
#include <grpcpp/channel.h>
#include <grpcpp/client_context.h>
#include <grpcpp/create_channel.h>
#include <grpcpp/security/credentials.h>
#include <iostream>
using namespace std;

//using grpc::ClientContext;
// using grpc::ClientReader;
// using grpc::ClientReaderWriter;
// using grpc::ClientWriter;
using io_channel::IoChannel;
using io_channel::SendRequest;
using io_channel::RetCode;


class ClientConnection
{
public:
	ClientConnection(const string& server_nodeid, const string& server_addr);
	virtual ~ClientConnection(){}

  	ssize_t send(const string& id, const string& data, int64_t timeout = -1L);
  	// ssize_t recv(const string& id, string& data, int64_t timeout = -1L);

private:
	string self_nodeid_ = "";
	unique_ptr<IoChannel::Stub> stub_;
};
