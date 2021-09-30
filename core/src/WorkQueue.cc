#if ASYNC_SERVER
#include <Ice/Ice.h>
#include <WorkQueue.h>

using namespace std;

WorkQueue::WorkQueue(const string& nodeid, shared_ptr<ClientConnection> ptr_client_conn) :
    _done(false), ptr_client_conn_(ptr_client_conn), nodeid_(nodeid)
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
                //// A response to a client request is advanced to a work queue
                // entry._cb->ice_response(0);
                ptr_client_conn_->write(entry._msgid, entry._data);
                _callbacks.pop_front();
            }
        }
    }

    //
    // Throw exception for any outstanding requests.
    //
    list<CallbackEntry>::const_iterator p;
    for(p = _callbacks.begin(); p != _callbacks.end(); ++p)
    {
        (*p)._cb->ice_exception(SendDataException());
    }
}

void WorkQueue::add(const AMD_IoChannel_sendPtr& cb, const string& msgid, const bytes& data)
{
    IceUtil::Monitor<IceUtil::Mutex>::Lock lock(_monitor);
    if(!_done)
    {
        cb->ice_response(0);
        //
        // Add work item.
        //
        CallbackEntry entry(cb, msgid, data);
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