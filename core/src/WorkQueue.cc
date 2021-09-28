#if ASYNC_SERVER
#include <Ice/Ice.h>
#include <WorkQueue.h>

using namespace std;

WorkQueue::WorkQueue(map<string, shared_ptr<ClientConnection>>* ptr_client_conn_map) :
    _done(false), ptr_client_conn_map_(ptr_client_conn_map)
{
}

void WorkQueue::run()
{
    IceUtil::Monitor<IceUtil::Mutex>::Lock lock(_monitor);
    while(!_done)
    {
        if(_callbacks.size() == 0)
        {
            _monitor.wait();
        }

        if(!_done && _callbacks.size() != 0)
        {
            //
            // Get next work item.
            //
            CallbackEntry entry = _callbacks.front();
            if(!_done)
            {
                auto iter = ptr_client_conn_map_->find(entry.nodeid);
                if(iter != ptr_client_conn_map_->end())
                {
                    iter->second->write(entry.msgid, entry.data);
                }
                _callbacks.pop_front();
                entry.cb->ice_response(0);
            }
        }
    }

    //
    // Throw exception for any outstanding requests.
    //
    list<CallbackEntry>::const_iterator p;
    for(p = _callbacks.begin(); p != _callbacks.end(); ++p)
    {
        (*p).cb->ice_exception(SendDataException());
    }
}

void WorkQueue::add(const AMD_IoChannel_sendPtr& cb, const string& nodeid, 
        const string& msgid, const bytes& data)
{
    IceUtil::Monitor<IceUtil::Mutex>::Lock lock(_monitor);
    if(!_done)
    {
        //
        // Add work item.
        //
        CallbackEntry entry;
        entry.cb = cb;
        entry.nodeid = nodeid;
        entry.msgid = msgid;
        entry.data = data;

        if(0 == _callbacks.size())
        {
            _monitor.notify();
        }
        _callbacks.push_back(entry);
    }
    else
    {
        //
        // Destroyed, throw exception.
        //
        cb->ice_exception(SendDataException());
    }
}

void WorkQueue::destroy()
{
    IceUtil::Monitor<IceUtil::Mutex>::Lock lock(_monitor);
    _done = true;
    _callbacks.clear();
    _monitor.notify();
}
#endif