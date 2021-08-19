
// file base_client.h
#pragma once
#include "config.h"
#include <string>
#include <functional>
#include <stdexcept>
#include <memory>
#include <iostream>
#include <cmath>
#include "assert.h"
#include "unistd.h"

#include <grpc++/grpc++.h>
#include <grpc/grpc.h>
#include <grpcpp/channel.h>
#include <grpcpp/client_context.h>
#include <grpcpp/create_channel.h>
#include <grpcpp/security/credentials.h>
#include <thread>

#include "io_channel.grpc.pb.h"
using grpc::Status;
using grpc::Channel;
using grpc::ClientContext;
using io_channel::IoChannel;
using io_channel::SendRequest;
using io_channel::RetCode;
using namespace std;

class BaseClient
{
public:
    BaseClient(const ViaInfo& via_info, const string& taskid);
	virtual ssize_t send(const string& self_nodeid, const string& remote_nodeid, 
	  	const string& msg_id, const char* data, const size_t nLen, int64_t timeout = -1L) = 0;

	virtual ~BaseClient(){}
public:
	string task_id_;
protected:
    // Out of the passed in Channel comes the stub, stored here, our view of the
    // server's exposed services.
    std::unique_ptr<IoChannel::Stub> stub_;
};
