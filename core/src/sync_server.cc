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
    cout << "SyncServer::SyncServer" << endl;
    Ice::ObjectPtr object = new IoChannelI(ptr_client_conn_map);
	Ice::Identity id ;
	id.name = "IoChannel";
	//增加一个适配器
	adapter_->add(object, id);
	//激发启动一个通信器
	adapter_->activate();
    cout << "SyncServer::SyncServer succeed=========" << endl;
	//通信器在这里等待处理数据连接
	// ic_->waitForShutdown();
}

int IoChannelI::send(const SendRequest& request, const Ice::Current&)
{
    auto iter = ptr_client_conn_map_->find(request.nodeid);
    if(iter == ptr_client_conn_map_->end())
    {
        return RET_SUCCEED_CODE;
    }
    // The msgid is already included in the data  
    const string& data = request.data;
#if USE_BUFFER
    iter->second->buffer_->write(data.data(), data.size());
#else
    const string& msgid = request.id;
    iter->second->write(msgid, data);
#endif
    return RET_SUCCEED_CODE;
}
