
// file base_server.h
#pragma once
#include "config.h"
#include "IoChannelI.h"
#include "client_connection.h"
#include <unistd.h>
#include <functional>
#include <stdexcept>
#include <memory>
#include <iostream>
#include <cmath>
#include <map>
#include "assert.h"
#include <Ice/Ice.h>
#include <io_channel_ice.h>
#include <thread>
using namespace ChannelSdk;
using namespace std;

class BaseServer
{
public:
	BaseServer(const NodeInfo& server_info, 
		map<string, shared_ptr<ClientConnection>>* ptr_client_conn_map);
	virtual ~BaseServer(){}

#if ASYNC_SERVER
	virtual void Handle_Event(const int numEvent) = 0;
	virtual int get_thread_count() = 0;
#if USE_CACHE
	#if MULTI_LOCKS
		virtual void Handle_Data(const string& nodeid) = 0;
	#else 
		virtual void Handle_Data() = 0;
	#endif
#endif
#endif

protected:
	virtual bool close()
	{
		if (ptr_communicator_) 
		{
			try {
				ptr_communicator_->destroy();
			} catch (const Ice::Exception& e) {
				cerr << e << endl;
			}
    	}
		return true;
	}

protected:
	shared_ptr<Ice::CommunicatorHolder> ptr_holder_;
	Ice::CommunicatorPtr ptr_communicator_;
	Ice::ObjectAdapterPtr ptr_adapter_;
	map<string, shared_ptr<ClientConnection>>* ptr_client_conn_map_ = nullptr;
};

