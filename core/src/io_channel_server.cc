// file io_channel_server.cc
#include "io_channel_server.h"

// 服务器
bool IoChannelServer::close()
{
    // 关闭服务
    if(server_)
        server_->Shutdown();
        
    cout << "IoChannelServer::close()" << endl;
}

bool IoChannelServer::wait()
{
    if(server_)
        server_->Wait();
    cout << "IoChannelServer::wait()" << endl;
}

std::map<string, string>& IoChannelServer::get_data_map()
{
    return save_data_map_;
}

IoChannelServer::IoChannelServer(const string& server_addr) 
{
    grpc::ServerBuilder builder;
    // 调整消息大小限制(默认最大为4M, 设置最大为2G)
    builder.SetMaxReceiveMessageSize(INT_MAX);
    builder.AddListeningPort(server_addr, grpc::InsecureServerCredentials());
    builder.RegisterService(this);
    server_ = builder.BuildAndStart();
    cout << "Server listening on " << server_addr << endl;
    // server_->Wait();
}

// 同步方式
grpc::Status IoChannelServer::Send(grpc::ServerContext* context, const SendRequest* request, 
    RetCode* response)
{
    std::lock_guard<std::mutex> guard(mtx_);
    cout << "send request nodeid:" << request->nodeid() << ", id:" << request->id() 
            << ", data length:" << request->data().length() << endl;

    // key = nodeid:msg_id
    string strSaveId = request->nodeid() + ":" + request->id();
    cout << "send strSaveId:" << strSaveId << endl;
    // 保存数据
    save_data_map_.insert(std::pair<string, string>(strSaveId, request->data()));
    response->set_code(0);
    // cout << "save_data_map_.size == " << save_data_map_.size() << endl;

    return Status::OK;
}

/*
// stream方式发送
grpc::Status Send(grpc::ServerContext* context, ServerReader<SendRequest>* request, 
        RetCode* response)
{
    std::lock_guard<std::mutex> guard(mtx_);
    SendRequest req;
    int i = 0;
    while (request->Read(&req)) 
    {
        i++;
        cout << "i: " << i << ", send request nodeid:" << req.nodeid() << ", id:" 
            << req.id() << ", data:" << req.data() <<  ", timeout:" << req.timeout() << endl;
    }

    response->set_code(100);
    return grpc::Status::OK;
}
*/

/*
grpc::Status Recv(grpc::ServerContext* context, const RecvRequest* request, RetData* respond)
{
    std::lock_guard<std::mutex> guard(mtx_);
    cout << "recv request nodeid:" << request->nodeid()  << ", id:" << request->id() << endl;
    string req_id = request->id() + request->nodeid();
    SendRequest send_req = __requests[req_id];
    respond->set_data(send_req.data());

    return grpc::Status::OK;
}*/
