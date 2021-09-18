
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

#include <Ice/Ice.h>
#include <io_channel_ice.h>
#include <thread>
using namespace ChannelSdk;

class BaseClient
{
private:
    // bool MakeCredentials(const ViaInfo& via_info);
public:
    BaseClient(const ViaInfo& via_info, const string& taskid);
    // // Create a connection between the node where the server is located and VIA to register the interface.
    // BaseClient(const NodeInfo& node_info, const string& taskid);
    // // Register the interface where the server node resides to via
    // bool SignUpToVia(const NodeInfo& server_info);

    bool CheckConnect(const useconds_t usec=1000000);

    virtual ssize_t send(const string& self_nodeid, const string& remote_nodeid, 
        const string& msg_id, const char* data, const size_t nLen, int64_t timeout = -1L) = 0;

    virtual ~BaseClient(){ if(ptr_communicator_) ptr_communicator_->destroy();}
public:
	string task_id_;
protected:
    shared_ptr<Ice::CommunicatorHolder> ptr_holder_;
    Ice::CommunicatorPtr ptr_communicator_;
    IoChannelPrx stub_;
};

