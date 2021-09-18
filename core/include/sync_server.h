// file sync_server.h
#pragma once
#include "base_server.h"
#include "config.h"
#include "client_connection.h"
#include <iostream>
#include <unordered_map>
#include <mutex>
#include<chrono>
#include<thread>
#include<map>
#include<queue>
#include <Ice/Ice.h>
#include "io_channel_ice.h"
using namespace ChannelSdk;
using namespace std;

// 同步服务器
class SyncServer: public BaseServer
{
public:
    bool wait();
    ~SyncServer(){close();}

    SyncServer(const NodeInfo& server_info, map<string, shared_ptr<ClientConnection>>* ptr_client_conn_map);
private:
    std::mutex mtx_;
};
