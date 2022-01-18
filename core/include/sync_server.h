// file sync_server.h
#pragma once
#ifndef ASYNC_SERVER
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
using namespace ChannelSdk;
using namespace std;

// 同步服务器
class SyncServer: public BaseServer
{
public:
    bool wait();
    ~SyncServer(){close();}

    SyncServer(const NodeInfo& server_info, const string& taskId, MapClientConn*);
};
#endif
