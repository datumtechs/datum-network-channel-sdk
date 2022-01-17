#include "IoChannelI.h"

#ifdef ASYNC_SERVER
IoChannelI::IoChannelI(MapWorkQueue* ptr_noide_to_wq_map):
  ptr_noide_to_wq_map_(ptr_noide_to_wq_map){}
#else
IoChannelI::IoChannelI(MapClientConn* ptr_client_conn_map):
  ptr_client_conn_map_(ptr_client_conn_map){}
#endif

void IoChannelI::send_async(const AMD_IoChannel_sendPtr& cb, const string& nodeid, 
      const string& msgid, const bytes& data, const Ice::Current&)
{
#if ASYNC_SERVER
    auto iter = ptr_noide_to_wq_map_->find(nodeid);
    if(iter != ptr_noide_to_wq_map_->end())
    {
        iter->second->add(cb, msgid, data);  
    }
#else
    auto iter = ptr_client_conn_map_->find(nodeid);
    if(iter != ptr_client_conn_map_->end())
    {
        iter->second->write(msgid, data);
    } 
    cb->ice_response(RET_SUCCEED_CODE);
#endif
}
