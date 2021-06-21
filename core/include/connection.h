// file connection.h
#pragma once
#include "io_channel_server.h"
#include <unordered_map>
#include <mutex>
#include <iostream>
using namespace std;

#include "io_channel.grpc.pb.h"
#include <grpc/grpc.h>
#include <grpcpp/channel.h>
#include <grpcpp/client_context.h>
#include <grpcpp/create_channel.h>
#include <grpcpp/security/credentials.h>
using grpc::Status;
using grpc::ClientContext;
using grpc::ClientReader;
using grpc::ClientReaderWriter;
using grpc::ClientWriter;
using io_channel::IoChannel;
using io_channel::SendRequest;
using io_channel::RetCode;

class ClientConnection
{
public:
	ClientConnection(const string& server_addr, const string& taskid);
	virtual ~ClientConnection(){}

  	ssize_t send(const string& self_nodeid, const string& remote_nodeid, 
	  	const string& task_id,  const string& id, const string& data, int64_t timeout = -1L);
  	// ssize_t recv(const string& id, string& data, int64_t timeout = -1L);
public:
	string task_id_;
private:
	
	unique_ptr<IoChannel::Stub> stub_;
};
