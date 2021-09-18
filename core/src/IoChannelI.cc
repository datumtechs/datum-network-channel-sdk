#include "IoChannelI.h"

IoChannelI::IoChannelI(map<string, shared_ptr<ClientConnection>>* ptr_client_conn_map):
    ptr_client_conn_map_(ptr_client_conn_map){}

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
