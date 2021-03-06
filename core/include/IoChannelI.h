#pragma once

#include "config.h"
#include <Ice/Ice.h>
#include "client_connection.h"
#include "io_channel_ice.h"
#include "WorkQueue.h"

using namespace ChannelSdk;

#if STATIC_CALL
class IoChannelI : public ChannelSdk::IoChannel 
#else
class IoChannelI : public Ice::Blobject
#endif
{
public:
#ifdef ASYNC_SERVER
   IoChannelI(MapWorkQueue*);
#else
   IoChannelI(MapClientConn*);
#endif

   // static call
#if STATIC_CALL
   virtual void send_async(const AMD_IoChannel_sendPtr&, const string& nodeid, 
      const string& msgid, const bytes& data, const Ice::Current&);
#else
   // dynamic call
   virtual bool ice_invoke(const std::vector<Ice::Byte>&, std::vector<Ice::Byte>&, const Ice::Current&);
#endif

private:
#ifdef ASYNC_SERVER
   // Each nodeID corresponds to a work queue, reducing queue query judgment,
   // The memory is created when the AsyncServer object is initialized, 
   // and this variable holds only memory Pointers.
   MapWorkQueue* ptr_noide_to_wq_map_ = nullptr;
#else
   MapClientConn* ptr_client_conn_map_ = nullptr;  
#endif
    
};
