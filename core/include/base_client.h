
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
#include "const.h"
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
    // Checking connection status (static call)
    bool CheckByStaticCall(const uint64_t conn_timeout=5000, const useconds_t usec=1000000);
    // Checking connection status (dynamic call)
    bool CheckByDynamicCall(const uint64_t conn_timeout=5000, const useconds_t usec=1000000);
    void SetSendTimeOut(const uint64_t send_timeout) {
        send_timeout_ = send_timeout;
    }

    virtual ssize_t send(const string& self_nodeid, const string& remote_nodeid, 
        const string& msg_id, const char* data, const size_t nLen, int64_t timeout = -1L) = 0;

    virtual ~BaseClient();
public:
	string task_id_;
protected:
    shared_ptr<Ice::CommunicatorHolder> ptr_holder_;
    Ice::CommunicatorPtr ptr_communicator_;
    IoChannelPrx stub_;
    uint64_t send_timeout_; // Timeout period for sending data each time, Unit:millisecond
    string remote_nid_;
};
 
