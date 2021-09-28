#pragma once
#if ASYNC_SERVER
#include "client_connection.h"
#include <io_channel_ice.h>
#include <Ice/Ice.h>
#include <list>
#include <string>
using namespace ChannelSdk;
using namespace std;

class WorkQueue : public IceUtil::Thread
{
public:
    WorkQueue(map<string, shared_ptr<ClientConnection>>* ptr_client_conn_map);
    virtual void run();
    void add(const AMD_IoChannel_sendPtr&, const string& nodeid, 
        const string& msgid, const bytes& data);

    void destroy();

private:

    struct CallbackEntry
    {
        AMD_IoChannel_sendPtr cb;
        string nodeid;
        string msgid;
        bytes data;
    };

    IceUtil::Monitor<IceUtil::Mutex> _monitor;
    std::list<CallbackEntry> _callbacks;
    bool _done;

    map<string, shared_ptr<ClientConnection>>* ptr_client_conn_map_ = nullptr;  
};

typedef IceUtil::Handle<WorkQueue> WorkQueuePtr;

#endif
