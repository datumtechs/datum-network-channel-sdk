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

using grpc::Status;
using grpc::ClientContext;
using grpc::ClientReader;
using grpc::ClientReaderWriter;
using grpc::ClientWriter;
using grpc::ClientAsyncResponseReader;
using grpc::CompletionQueue;
using io_channel::IoChannel;
using io_channel::SendRequest;
using io_channel::RetCode;

class ClientConnection
{
public:
	ClientConnection(const string& server_addr);
	virtual ~ClientConnection(){}

  	ssize_t send(const string& self_nodeid, const string& remote_nodeid, 
	  	const string& task_id,  const string& id, const string& data, int64_t timeout = -1L);
  	// ssize_t recv(const string& id, string& data, int64_t timeout = -1L);

	// bool AsyncCompleteRpc();

private:
	/*
	// struct for keeping state and data information
	struct AsyncClientCall {
		// Container for the data we expect from the server.
		RetCode reply;

		// Context for the client. It could be used to convey extra information to
		// the server and/or tweak certain RPC behaviors.
		ClientContext context;

		// Storage for the status of the RPC upon completion.
		Status status;

		std::unique_ptr<ClientAsyncResponseReader<RetCode>> response_reader;
	};
	*/

	unique_ptr<IoChannel::Stub> stub_;
	// The producer-consumer queue we use to communicate asynchronously with the
	// gRPC runtime.
	CompletionQueue cq_;
};
