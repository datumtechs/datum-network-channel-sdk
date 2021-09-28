#include "IoChannelI.h"

#ifdef ASYNC_SERVER
IoChannelI::IoChannelI(const WorkQueuePtr& workQueue):ptr_workQueue_(workQueue){}
#else
IoChannelI::IoChannelI(map<string, shared_ptr<ClientConnection>>* ptr_client_conn_map):
  ptr_client_conn_map_(ptr_client_conn_map){}
#endif

void IoChannelI::send_async(const AMD_IoChannel_sendPtr& cb, const string& nodeid, 
      const string& msgid, const bytes& data,const Ice::Current&)
{
#if ASYNC_SERVER
    ptr_workQueue_->add(cb, nodeid, msgid, data);  
#else
    auto iter = ptr_client_conn_map_->find(nodeid);
    if(iter != ptr_client_conn_map_->end())
    {
        iter->second->write(msgid, data);
    }
    cb->ice_response(RET_SUCCEED_CODE);
#endif
}
