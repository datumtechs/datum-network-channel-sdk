// file sync_client.cc
#include "sync_client.h"
#include <thread>
#include <chrono>   
using namespace chrono;

SyncClient::SyncClient(const ViaInfo& via_info, const string& taskid):
  BaseClient(via_info, taskid){}

// SyncClient::SyncClient(const NodeInfo& node_info, const string& taskid):
//   BaseClient(node_info, taskid){}

ssize_t SyncClient::send(const string& self_nodeid, const string& remote_nodeid, 
    const string& msg_id, const char* data, const size_t nLen, int64_t timeout)
{
  auto start_time = system_clock::now();
  auto end_time   = start_time;
  int64_t elapsed = 0;
  Ice::ByteSeq vec_send_data;
  vec_send_data.resize(nLen);
  memcpy(&vec_send_data[0], data, nLen);

  Ice::Context ctx;
  ctx.insert(std::pair<string, string>("nodeid", self_nodeid));
  ctx.insert(std::pair<string, string>("msgid", msg_id));

  do {
    int status = 0;
    try{
      // Ice::Context context;
      // status = stub_->send(self_nodeid, msg_id, vec_send_data, context);
      Ice::ByteSeq inParams, outParams;
      Ice::OutputStream out(ptr_communicator_);
      out.startEncapsulation();
      out.write(vec_send_data);
      out.endEncapsulation();
      out.finished(inParams);
      if(!stub_->ice_invoke("send", Ice::Normal, inParams, outParams, ctx))
      {
          cout << "Send data to remote node:" << remote_nid_ << " failed, wait..." << endl;
          status = 1;
      }
    } catch (const Ice::Exception& ex) {
        cerr << ex << endl;
        status = 1;
    }
    
    if (0 == status) 
    {
      break;
    } 
    else 
    {
      end_time = system_clock::now();
      elapsed = duration_cast<duration<int64_t, std::milli>>(end_time - start_time).count();
      if(elapsed >= send_timeout_)
      {        
        string strErrMsg = "self nodeid:" + self_nodeid + " send data to nodeid:" + remote_nodeid + 
          " timeout, The timeout period is: " + to_string(send_timeout_) + "ms.";
        cout << strErrMsg << endl;
        throw (strErrMsg);
        // return 0;
      }
      sleep(1);
    }
  } while(true);

	return nLen;
}
