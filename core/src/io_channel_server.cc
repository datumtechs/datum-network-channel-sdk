// file io_channel_server.cc
#include "io_channel_server.h"
#include "simple_buffer.h"

// 服务器
bool IoChannelServer::close()
{
    // 关闭服务
    if(server_)
        server_->Shutdown();
}

bool IoChannelServer::wait()
{
    if(server_)
        server_->Wait();
    cout << "IoChannelServer::wait()" << endl;
}

IoChannelServer::IoChannelServer(const string& server_addr, 
        map<string, shared_ptr<ClientConnection>>* ptr_client_conn_map) 
{
    ptr_client_conn_map_ = ptr_client_conn_map;
    grpc::ServerBuilder builder;
    // 调整消息大小限制(默认最大为4M, 设置最大为2G)
    builder.SetMaxReceiveMessageSize(INT_MAX);
    builder.AddListeningPort(server_addr, grpc::InsecureServerCredentials());
    builder.RegisterService(this);
    server_ = builder.BuildAndStart();
    cout << "Server listening on " << server_addr << endl;
    // server_->Wait();
}

grpc::Status IoChannelServer::Send(grpc::ServerContext* context, const SendRequest* request, 
    RetCode* response)
{
    // cout << "IoChannelServer::Send, send request nodeid:" << nodeId << ", msgid:" << msgid << endl;
    std::unique_lock<mutex> guard(mtx_);
    const string& nodeId = request->nodeid();
    // const string& msgid = request->id();

    auto iter = ptr_client_conn_map_->find(nodeId);
    if(iter == ptr_client_conn_map_->end())
    {
        return Status::OK;
    }
    // The msgid is already included in the data  
    const string& data = request->data();
    iter->second->buffer_->write(data.data(), data.size());

    // simple_buffer buffer(msgid, data.data(), data.size());
    // client_conn->buffer_->write((const char*)buffer.data(), buffer.len());

    response->set_code(RET_SUCCEED_CODE);
    return Status::OK;
}
