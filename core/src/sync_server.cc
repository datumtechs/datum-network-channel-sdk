// file sync_server.cc
#include "sync_server.h"
#if USE_BUFFER
#include "simple_buffer.h"
#endif

bool SyncServer::wait()
{
    //通信器在这里等待处理数据连接
    if(ic_)
        ic_->waitForShutdown();

    return true;
}

SyncServer::SyncServer(const NodeInfo& server_info, 
    map<string, shared_ptr<ClientConnection>>* ptr_client_conn_map):
    BaseServer(server_info, ptr_client_conn_map)
{
    Ice::ObjectPtr object = new IoChannelI(ptr_client_conn_map);
	Ice::Identity id ;
	id.name = "IoChannel";
	//增加一个适配器
	adapter_->add(object, id);
	//激发启动一个通信器
	adapter_->activate();
	//通信器在这里等待处理数据连接
	// ic_->waitForShutdown();
}

// int IoChannelI::send(const SendRequest& request, const Ice::Current&)
int IoChannelI::send(const string& nodeid, const string& msgid, 
        const bytes& data, const Ice::Current&)
{
    auto iter = ptr_client_conn_map_->find(nodeid);
    if(iter == ptr_client_conn_map_->end())
    {
        return RET_SUCCEED_CODE;
    }

    iter->second->write(msgid, data);
    return RET_SUCCEED_CODE;
}
