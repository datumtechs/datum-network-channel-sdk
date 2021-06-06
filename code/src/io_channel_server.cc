// file io_channel_server.cc
#pragma once
#include "protos/io_channel.grpc.pb.h"
#include <iostream>
#include <unordered_map>
#include <mutex>
#include <grpc/grpc.h>
#include <grpc++/server.h>
#include <grpc++/server_builder.h>
#include <grpc++/server_context.h>
#include <grpc++/security/server_credentials.h>
#include <iostream>
#include<chrono>
#include<thread>
using namespace std;
using io_channel::IoChannel;
using io_channel::SendRequest;
using io_channel::RetCode;

class IoChannelServer final : public IoChannel::Service
{
public:
    bool close()
    {
        // 关闭服务
        if(server_)
            server_->Shutdown();
    }

    bool wait()
    {
        if(server_)
            server_->Wait();
    }

    std::map<string, string>& get_data_map()
    {
        return save_data_map_;
    }

    IoChannelServer(const string& server_addr) 
    {
        // 服务器
		grpc::ServerBuilder builder;
		builder.AddListeningPort(server_addr, grpc::InsecureServerCredentials());
		builder.RegisterService(this);
		server_ = builder.BuildAndStart();
		cout << "Server listening on " << server_addr << endl;
		// server_->Wait();
    }
    // 同步方式
	grpc::Status Send(grpc::ServerContext* context, const SendRequest* request, RetCode* response)
	{
		std::lock_guard<std::mutex> guard(mtx_);
		cout << "send request nodeid:" << request->nodeid() << ", id:" << request->id() 
             << ", data:" << request->data() <<  ", timeout:" << request->timeout() << endl;

        // key = nodeid:msg_id
        string strSaveId = request->nodeid() + ":" + request->id();
        cout << "send strSaveId:" << strSaveId << endl;
        // 保存数据
        save_data_map_.insert(std::pair<string, string>(strSaveId, request->data()));
        response->set_code(100);
        cout << "save_data_map_.size == " << save_data_map_.size() << endl;
        /*
        // req_id = id + nodeid
        string req_id = request->id() + request->nodeid();
        __requests[req_id] = *request;

		response->set_code(100);
        this_thread::sleep_for(std::chrono::milliseconds(10000));
        cout << "Sleep over" << endl;*/
        return grpc::Status::OK;
	}
	
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

    // 异步方式
private:
    std::mutex                    mtx_;
    std::unique_ptr<grpc::Server> server_;
    std::map<string, string> save_data_map_;
};
/*
int main(int argc, char *argv[])
{
    std::string addr = "0.0.0.0:6000";
    IoChannelServer server(addr);
    return 0;
}
*/
