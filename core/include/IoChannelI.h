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
   IoChannelI(unordered_map<string, WorkQueuePtr>*);
#else
   IoChannelI(unordered_map<string, shared_ptr<ClientConnection>>*);
#endif

   virtual void send_async(const AMD_IoChannel_sendPtr&, const string& nodeid, 
      const string& msgid, const bytes& data, const Ice::Current&);

private:
#ifdef ASYNC_SERVER
   // Each nodeID corresponds to a work queue, reducing queue query judgment,
   // The memory is created when the AsyncServer object is initialized, 
   // and this variable holds only memory Pointers.
   unordered_map<string, WorkQueuePtr>* ptr_noide_to_wq_map_ = nullptr;
#else
   unordered_map<string, shared_ptr<ClientConnection>>* ptr_client_conn_map_ = nullptr;  
#endif
    
};
