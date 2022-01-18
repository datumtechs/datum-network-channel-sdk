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
    WorkQueue(const string& nodeid, shared_ptr<ClientConnection> ptr_client_conn);
    virtual void run();
    void add(const AMD_IoChannel_sendPtr&, const string& msgid, const bytes& data);

    void destroy();

private:

    struct CallbackEntry
    {
        CallbackEntry(const AMD_IoChannel_sendPtr& cb,
            const string& msgid, const bytes& data):_cb(cb), _msgid(msgid), _data(data){}

        AMD_IoChannel_sendPtr _cb;
        string _msgid;
        bytes _data;
    };

    IceUtil::Monitor<IceUtil::Mutex> _monitor;
    std::list<CallbackEntry> _callbacks;
    bool _done;
    shared_ptr<ClientConnection> ptr_client_conn_; 
    string nodeid_;
};

typedef IceUtil::Handle<WorkQueue> WorkQueuePtr;
using MapWorkQueue = unordered_map<string, WorkQueuePtr>;

#endif
