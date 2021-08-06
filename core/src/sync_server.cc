// file sync_server.cc
#include "sync_server.h"
#if USE_BUFFER_
#include "simple_buffer.h"
#endif

// 服务器
bool SyncServer::close()
{
    // 关闭服务
    if(server_)
        server_->Shutdown();
}

bool SyncServer::wait()
{
    if(server_)
        server_->Wait();
    cout << "SyncServer::wait()" << endl;
}

SyncServer::SyncServer(const string& server_addr, 
    map<string, shared_ptr<ClientConnection>>* ptr_client_conn_map,
    const char* root_crt, const char* server_key, const char* server_cert)
{
    ptr_client_conn_map_ = ptr_client_conn_map;
    grpc::ServerBuilder builder;
    // 调整消息大小限制(默认最大为4M, 设置最大为2G)
    builder.SetMaxReceiveMessageSize(INT_MAX);
    shared_ptr<grpc::ServerCredentials> creds;
    
#ifdef SSL_TYPE
    if(0 == SSL_TYPE)
    {
        creds = grpc::InsecureServerCredentials();
    }
    else if(1 == SSL_TYPE)
    {
        if(nullptr == root_crt || nullptr == server_key || nullptr == server_cert)
        {
            cerr << "Invalid server certificate, please check!" << endl;
            return;
        }
        grpc::SslServerCredentialsOptions::PemKeyCertPair pkcp = {server_key, server_cert};
        grpc::SslServerCredentialsOptions ssl_opts(GRPC_SSL_REQUEST_AND_REQUIRE_CLIENT_CERTIFICATE_AND_VERIFY);
        ssl_opts.pem_root_certs = root_crt;
        ssl_opts.pem_key_cert_pairs.push_back(pkcp);
        creds = grpc::SslServerCredentials(ssl_opts);
    }
    else if(2 == SSL_TYPE)
    {
        
    }
#endif

    builder.AddListeningPort(server_addr, creds);
    builder.RegisterService(this);
    server_ = builder.BuildAndStart();
    cout << "Server listening on " << server_addr << endl;
    // server_->Wait();
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
#if USE_BUFFER_
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
