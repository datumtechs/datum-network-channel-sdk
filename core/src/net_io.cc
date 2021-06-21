#include "net_io.h"
#include "IChannel.h"
#include <unistd.h>
#include <set>
#include <chrono>   
using namespace chrono;

void BasicIO::close() {}

BasicIO::~BasicIO(){close();}

BasicIO::BasicIO(const NodeInfo &node_info, const vector<ViaInfo>& server_infos,
    map<string, string>* share_data_map, error_callback error_callback)
  : node_info_(node_info), via_server_infos_(server_infos), 
    share_data_map_(share_data_map), handler(error_callback) {}

bool ViaNetIO::init(const string& taskid) 
{
  // 创建多线程
  vector<thread> client_threads(via_server_infos_.size());
  vector<shared_ptr<ClientConnection>> clients(via_server_infos_.size());
  
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
    
    if(share_data_map_->find(strSaveId) != share_data_map_->end())
    {
      cout << "ViaNetIO::recv data111111 = " << data << endl;
      mutex mtx_;
      lock_guard<mutex> guard(mtx_);
      memcpy(data, (*share_data_map_)[strSaveId].c_str(), length);
      // data = (*share_data_map_)[strSaveId];
      cout << "ViaNetIO::recv data222222 = " << data << endl;
      // 删除数据
      share_data_map_->erase(strSaveId);
      break;
    }
    else
    {
      end_time = system_clock::now();
			auto duration = duration_cast<microseconds>(end_time - start_time);
			auto cost_time = double(duration.count()) * 
				microseconds::period::num / microseconds::period::den;

			cout << "recv data from " << remote_nodeid << " failed, cost " 
				 << cost_time << " seconds." << endl;
			if(cost_time >= timeout)
				break;
      std::this_thread::yield();
      sleep(SLEEP_TIME);
    }
  }
  
  return length;
}

ssize_t ViaNetIO::send(const string& remote_nodeid, const char* id, const char* data, 
      uint64_t length, int64_t timeout) 
{
  cout << "ViaNetIO::send, remote node_id: " << remote_nodeid << ", task id: " 
       << connection_map[remote_nodeid]->task_id_ << ", id: " << id << endl;
  
  ssize_t ret = connection_map[remote_nodeid]->send(node_info_.id, remote_nodeid, 
      connection_map[remote_nodeid]->task_id_, id, data, length, timeout);
  return ret;
}
