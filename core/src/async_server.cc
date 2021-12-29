// file async_server.cc
#ifdef ASYNC_SERVER
#include "async_server.h"

AsyncServer::~AsyncServer()
{
    // ptr_workQueue_->getThreadControl().join();
    for(auto &v : map_noide_to_wq_)
    {
        v.second->destroy();
    }
    close();
}

bool AsyncServer::wait()
{
    //通信器在这里等待处理数据连接
    if(ptr_communicator_)
        ptr_communicator_->waitForShutdown();

    return true;
}

AsyncServer::AsyncServer(const NodeInfo& server_info, 
    unordered_map<string, shared_ptr<ClientConnection>>* ptr_client_conn_map): BaseServer(server_info)
{
    for(auto &v : *ptr_client_conn_map)
    {
        // Create a work queue corresponding to the nodeID
        map_noide_to_wq_[v.first] = new WorkQueue(v.first, v.second);
    }

	string proxy_name = "IoChannel";
	Ice::Identity id ;
	id.name = proxy_name;
	//增加一个适配器
    IoChannelPtr object = new IoChannelI(&map_noide_to_wq_);
	ptr_adapter_->add(object, id);
    // 启动线程池
    for(auto &v : map_noide_to_wq_)
    {
        v.second->start();
    }
	//激发启动一个通信器
	ptr_adapter_->activate();
	//通信器在这里等待处理数据连接
	// ptr_communicator_->waitForShutdown();
}

#endif
