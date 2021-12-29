// file sync_server.cc
#include "sync_server.h"
#ifndef ASYNC_SERVER
bool SyncServer::wait()
{
    //通信器在这里等待处理数据连接
    if(ptr_communicator_)
        ptr_communicator_->waitForShutdown();

    return true;
}

SyncServer::SyncServer(const NodeInfo& server_info, 
    unordered_map<string, shared_ptr<ClientConnection>>* ptr_client_conn_map): BaseServer(server_info)
{
	string proxy_name = "IoChannel";
	Ice::Identity id ;
	id.name = proxy_name;
	//增加一个适配器
    Ice::ObjectPtr object = new IoChannelI(ptr_client_conn_map);
	ptr_adapter_->add(object, id);
	//激发启动一个通信器
	ptr_adapter_->activate();
	//通信器在这里等待处理数据连接
	// ptr_communicator_->waitForShutdown();
}
#endif