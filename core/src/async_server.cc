// file async_server.cc
#ifdef ASYNC_SERVER
#include "async_server.h"

AsyncServer::~AsyncServer()
{
    // ptr_workQueue_->getThreadControl().join();
    ptr_workQueue_->destroy();
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
    map<string, shared_ptr<ClientConnection>>* ptr_client_conn_map):
    BaseServer(server_info, ptr_client_conn_map)
{
    ptr_workQueue_ = new WorkQueue(ptr_client_conn_map);

	string proxy_name = "IoChannel";
	Ice::Identity id ;
	id.name = proxy_name;
	//增加一个适配器
    IoChannelPtr object = new IoChannelI(ptr_workQueue_);
	ptr_adapter_->add(object, id);
    // 启动线程池
    ptr_workQueue_->start();
	//激发启动一个通信器
	ptr_adapter_->activate();
	//通信器在这里等待处理数据连接
	// ptr_communicator_->waitForShutdown();
}

#endif
