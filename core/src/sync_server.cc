// file sync_server.cc
#include "sync_server.h"
#if USE_BUFFER
#include "simple_buffer.h"
#endif

bool SyncServer::wait()
{
    if(base_server_)
        base_server_->Wait();
    return true;
}

SyncServer::SyncServer(const NodeInfo& server_info, 
    map<string, shared_ptr<ClientConnection>>* ptr_client_conn_map):
    BaseServer(server_info, ptr_client_conn_map)
{
    builder_->RegisterService(this);
    base_server_ = builder_->BuildAndStart();
    gpr_log(GPR_INFO, "Sync Server listening on: %s.", server_info.address.c_str()); 
    // base_server_->Wait();
}

grpc::Status SyncServer::Send(grpc::ServerContext* context, const SendRequest* request, 
    RetCode* response)
{
    // cout << "SyncServer::Send, send request nodeid:" << nodeId << ", msgid:" << msgid << endl;
    // std::unique_lock<mutex> guard(mtx_);
    const string& nodeId = request->nodeid();
    // const string& msgid = request->id();

    auto iter = ptr_client_conn_map_->find(nodeId);
    if(iter == ptr_client_conn_map_->end())
    {
        return Status::OK;
    }
    // The msgid is already included in the data  
    const string& data = request->data();
#if USE_BUFFER
    iter->second->buffer_->write(data.data(), data.size());
#else
    const string& msgid = request->id();
    iter->second->write(msgid, data);
#endif
    // simple_buffer buffer(msgid, data.data(), data.size());
    // client_conn->buffer_->write((const char*)buffer.data(), buffer.len());

    response->set_code(RET_SUCCEED_CODE);
    return Status::OK;
}
