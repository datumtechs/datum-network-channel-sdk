#include "net_io.h"
#include "IChannel.h"
#include <unistd.h>
#include <set>
#include <chrono>   
using namespace chrono;

void BasicIO::close() {}

BasicIO::~BasicIO(){close();}

BasicIO::BasicIO(const NodeInfo &node_info, const vector<ViaInfo>& server_infos,
    map<string, shared_ptr<queue<string>>>* share_data_map, error_callback error_callback)
  : node_info_(node_info), via_server_infos_(server_infos), 
    share_data_map_(share_data_map), handler(error_callback) {}

bool ViaNetIO::init(const string& taskid) 
{
  vector<shared_ptr<ClientConnection>> clients(via_server_infos_.size());
  for (int i = 0; i < via_server_infos_.size(); i++) 
  {
    clients[i] = make_shared<ClientConnection>(via_server_infos_[i].address, taskid);
  }
  /*
  // 创建多线程
  vector<thread> client_threads(via_server_infos_.size());
  auto conn_f = [&](const ViaInfo &info, int i) -> bool {
    // 创建io通道
    cout << "create io channel, sids:" << info.id << endl;
    clients[i] = make_shared<ClientConnection>(info.address, taskid);
    return true;
  };

  for (int i = 0; i < via_server_infos_.size(); i++) {
    client_threads[i] = thread(conn_f, via_server_infos_[i], i);
  }

  for (int i = 0; i < via_server_infos_.size(); i++) {
    client_threads[i].join();
  }
  */

  for (int i = 0; i < via_server_infos_.size(); i++)
  {
    string ser_node_id =  via_server_infos_[i].id;
    // 保存到connection_map
    cout << "save io channel sids:" << via_server_infos_[i].id << endl;
    connection_map[ser_node_id] = clients[i];
  }
  
  cout << "init all network connections succeed!" << endl;
  return true;
}

ssize_t ViaNetIO::recv(const string& remote_nodeid, const char* id, char* data,
      uint64_t length, int64_t timeout) 
{
  auto recv_f = [&]() -> bool {
    string msg_id(id); 
    string strSaveId = remote_nodeid + ":" + msg_id;
    // cout << "recv strSaveId:" << strSaveId << endl;
    // cout << "share_data_map_->size == " << share_data_map_->size() << endl;
    // 从本地缓存中获取
    // 计时
    auto start_time = system_clock::now();
    auto end_time   = start_time;
    while(true)
    {
      auto iter = share_data_map_->find(strSaveId);
      if(iter != share_data_map_->end())
      {
        mutex mtx_;
        std::unique_lock<mutex> lck(mtx_);
        // memcpy(data, (*share_data_map_)[strSaveId].data(), length);
        shared_ptr<queue<string>> ptr_data_queue = iter->second;
        if(!ptr_data_queue->empty())
        {
          string strData = ptr_data_queue->front();
          uint64_t nSize = strData.size();
          memcpy(data, strData.data(), nSize);
          // cout << "ViaNetIO::recv succee, data size = " <<  nSize << ", length = " << length << endl;
          length = nSize;
          ptr_data_queue->pop();
          // 删除数据
          // share_data_map_->erase(strSaveId);
          break;
        }
      }
      else
      {
        end_time = system_clock::now();
        auto duration = duration_cast<microseconds>(end_time - start_time);
        auto cost_time = double(duration.count()) * 
          microseconds::period::num / microseconds::period::den;

        // cout << "recv data from " << remote_nodeid << " failed, cost " 
        //   << cost_time << " seconds." << endl;
        if(cost_time >= timeout)
          break;
        std::this_thread::yield();
        // this_thread::sleep_for(chrono::milliseconds(SLEEP_TIME_MILLI_SECONDS));
      }
    }
    return true;
  };

  thread recv_thread = thread(recv_f);
  recv_thread.join();  
  return length;
}

ssize_t ViaNetIO::send(const string& remote_nodeid, const char* id, const char* data, 
      uint64_t length, int64_t timeout) 
{
  // cout << "ViaNetIO::send, remote node_id: " << remote_nodeid << ", task id: " 
  //      << connection_map[remote_nodeid]->task_id_ << ", id: " << id << endl;
  
  ssize_t ret = connection_map[remote_nodeid]->send(node_info_.id, remote_nodeid, 
      connection_map[remote_nodeid]->task_id_, id, data, length, timeout);

  return ret;
}
