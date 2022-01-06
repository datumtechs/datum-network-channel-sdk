
// file base_server.h
#pragma once
#include "config.h"
#include "const.h"
#include "IoChannelI.h"
#include "client_connection.h"
#include <unistd.h>
#include <functional>
#include <stdexcept>
#include <memory>
#include <iostream>
#include <cmath>
#include "assert.h"
#include <Ice/Ice.h>
#include <io_channel_ice.h>
#include <thread>
using namespace ChannelSdk;
using namespace std;

class BaseServer
{
public:
	BaseServer(const NodeInfo& server_info, const string& taskId);
	virtual ~BaseServer(){}

protected:
	virtual bool close()
	{
		if (ptr_communicator_) 
		{
			try {
				ptr_communicator_->destroy();
				ptr_communicator_->shutdown();
			} catch (const Ice::Exception& e) {
				cerr << e << endl;
			}
    	}
		return true;
	}

protected:
	string taskId_;
	shared_ptr<Ice::CommunicatorHolder> ptr_holder_;
	Ice::CommunicatorPtr ptr_communicator_;
	Ice::ObjectAdapterPtr ptr_adapter_;
	unordered_map<string, shared_ptr<ClientConnection>>* ptr_client_conn_map_ = nullptr;
};

