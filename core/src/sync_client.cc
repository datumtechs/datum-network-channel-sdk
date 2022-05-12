// file sync_client.cc
#include "sync_client.h"
#include <thread>
#include <chrono>   
using namespace chrono;

SyncClient::SyncClient(const ViaInfo& via_info, const string& taskid):
  BaseClient(via_info, taskid){}

ssize_t SyncClient::send(const string& self_nodeid, const string& remote_nodeid, 
    const string& msg_id, const char* data, const size_t nLen, int64_t timeout)
{
  timer_.start();
#if STATIC_CALL
  Ice::ByteSeq vec_send_data;
  vec_send_data.resize(nLen);
  memcpy(&vec_send_data[0], data, nLen);
#else
  ChannelSdk::DataStruct sendData;
  sendData.msgid = msg_id;
  sendData.nodeid = self_nodeid;
  sendData.data.resize(nLen);
  memcpy(&sendData.data[0], data, nLen);
#endif

  do {
    try {
      // static call
    #if STATIC_CALL
      Ice::Context context;
      stub_->send(self_nodeid, msg_id, vec_send_data, context);
    #else
      // dynamic call
      Ice::ByteSeq inParams, outParams;
      Ice::OutputStream out(ptr_communicator_);
      out.startEncapsulation();
      out.write(sendData);
      out.endEncapsulation();
      out.finished(inParams);
      stub_->ice_invoke("send", Ice::Normal, inParams, outParams);
    #endif
      break;
    } catch (const Ice::Exception& ex) {
      // cerr << ex << endl;
      if(timer_.ms_elapse() >= send_timeout_)
      {
        HANDLE_EXCEPTION_EVENT(C_EVENT_CODE_SEND_DATA_TIMEOUT, task_id_, self_nodeid.c_str(), 
            remote_nodeid.c_str(), send_timeout_);
      }
      sleep(1);
    }
  } while(true);

	return nLen;
}
