#pragma once

#include "config.h"
#include <Ice/Ice.h>
#include "client_connection.h"
#include "io_channel_ice.h"
#include "WorkQueue.h"

using namespace ChannelSdk;

class IoChannelI : public ChannelSdk::IoChannel 
{
public:
#ifdef ASYNC_SERVER
   IoChannelI(const WorkQueuePtr&);
#else
   IoChannelI(map<string, shared_ptr<ClientConnection>>* ptr_client_conn_map);
#endif

   virtual void send_async(const AMD_IoChannel_sendPtr&, const string& nodeid, 
      const string& msgid, const bytes& data, const Ice::Current&);

private:
#ifdef ASYNC_SERVER
   WorkQueuePtr ptr_workQueue_;
#else
   map<string, shared_ptr<ClientConnection>>* ptr_client_conn_map_ = nullptr;  
#endif
    
};
