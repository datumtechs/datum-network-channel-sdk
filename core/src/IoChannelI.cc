#include "IoChannelI.h"

#ifdef ASYNC_SERVER
IoChannelI::IoChannelI(unordered_map<string, WorkQueuePtr>* ptr_noide_to_wq_map):
  ptr_noide_to_wq_map_(ptr_noide_to_wq_map){}
#else
IoChannelI::IoChannelI(unordered_map<string, shared_ptr<ClientConnection>>* ptr_client_conn_map):
  ptr_client_conn_map_(ptr_client_conn_map){}
#endif


bool IoChannelI::ice_invoke(const vector<Ice::Byte>& inParams, vector<Ice::Byte>& outParams, 
    const Ice::Current& current) 
{
  if("send" == current.operation)
  {
      Ice::CommunicatorPtr communicator = current.adapter->getCommunicator();
      Ice::InputStream in(communicator, inParams);
      in.startEncapsulation();

      Ice::Context ctx = current.ctx;
      string nodeid = ctx["nodeid"];
      string msgid = ctx["msgid"];
      // cout << "recv from nodeid:" << nodeid << ", msgid:" << msgid << endl;
      bytes data;
      in.read(data);
      auto iter = ptr_client_conn_map_->find(nodeid);
      if(iter != ptr_client_conn_map_->end())
      {
        iter->second->write(msgid, data);
      }
      in.endEncapsulation(); 
  }
  else if("ping" == current.operation)
  {
    // do nothing
    // cout << "recv ping message===" << endl;
  }
  else
  {
      Ice::OperationNotExistException ex(__FILE__, __LINE__);
      ex.id = current.id;
      ex.facet = current.facet;
      ex.operation = current.operation;
      throw ex;
  }

  return true;
}

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
