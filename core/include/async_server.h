
// file async_server.h
#pragma once
#if ASYNC_SERVER
#include "base_server.h"
#include "config.h"
#include "client_connection.h"
#include <iostream>
#include <unordered_map>
#include <mutex>
#include<chrono>
#include<thread>
#include<queue>
#include <Ice/Ice.h>
#include "io_channel_ice.h"
#include "WorkQueue.h"
using namespace ChannelSdk;
using namespace std;

// 异步同步服务器
class AsyncServer: public BaseServer
{
public:
    bool wait();
    ~AsyncServer();

    AsyncServer(const NodeInfo& server_info, const string& taskId, MapClientConn* ptr_client_conn_map);
private:
    // Each nodeID corresponds to a work queue, reducing queue query judgment.
    MapWorkQueue map_noide_to_wq_;
};
#endif

