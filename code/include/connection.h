// file connection.h
#pragma once

#include "protos/io_channel.grpc.pb.h"
#include "src/io_channel_server.cc"
#include <unordered_map>
#include <mutex>
#include <grpc/grpc.h>
#include <grpc++/server.h>
#include <grpc++/server_builder.h>
#include <grpc++/server_context.h>
#include <grpc++/security/server_credentials.h>
#include <grpc++/channel.h>
#include <grpc++/client_context.h>
#include <grpc++/create_channel.h>
#include <grpc++/security/credentials.h>
#include <iostream>
using namespace std;
using io_channel::IoChannel;
using io_channel::SendRequest;
using io_channel::RetCode;
using io_channel::TestReply;

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
