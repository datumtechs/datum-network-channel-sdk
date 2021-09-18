#pragma once

#include "config.h"
#include <Ice/Ice.h>
#include "client_connection.h"
#include "io_channel_ice.h"

using namespace ChannelSdk;

class IoChannelI : public ChannelSdk::IoChannel 
{
public:
    IoChannelI(map<string, shared_ptr<ClientConnection>>* ptr_client_conn_map);

    virtual int send(const string& nodeid, const string& msgid, 
        const bytes& data, const Ice::Current&);

private:
    map<string, shared_ptr<ClientConnection>>* ptr_client_conn_map_ = nullptr;  
};
