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

SyncServer::SyncServer(const NodeInfo& server_info, const string& taskId,
	MapClientConn* ptr_client_conn_map): BaseServer(server_info, taskId)
{
	Ice::Identity id ;
	id.name = C_Servant_Id_Prefix + taskId + "_" + server_info.id;
	FilterIllChar(id.name);
	try 
	{
		//增加一个适配器
		Ice::ObjectPtr object = new IoChannelI(ptr_client_conn_map);
		ptr_adapter_->add(object, id);
		//激发启动一个通信器
		ptr_adapter_->activate();
		//通信器在这里等待处理数据连接
		// ptr_communicator_->waitForShutdown();
	} 
	catch (const Ice::Exception& e) 
	{
		HANDLE_EXCEPTION_EVENT(C_EVENT_CODE_START_SERVICE, taskId, server_info.id.c_str(), e.what());
	}
}
#endif