#include "grpc_channel.h"

const char* encode_string(const string& str) {
  int len = sizeof(int) + str.size();
  char* res = new char[len];
  memcpy(res, &len, sizeof(int));
  memcpy(res + sizeof(int), str.data(), str.size());
  return res;
}
string decode_string(const char* str) {
  int len = (*(int*)str);
  string res;
  res.resize(len - sizeof(int));
  memcpy(&res[0], str + sizeof(int), len - sizeof(int));
  return res;
}
const char* encode_vector(const vector<string>& vec) {
  int len = sizeof(int);
  for (int i = 0; i < vec.size(); i++) {
    len += sizeof(int) + vec[i].size();
  }
  char* res = new char[len];
  int offset = 0;
  int num = vec.size();
  memcpy(res + offset, &num, sizeof(int));
  offset += sizeof(int);
  for (int i = 0; i < vec.size(); i++) {
    int len2 = sizeof(int) + vec[i].size();
    memcpy(res + offset, &len2, sizeof(int));
    memcpy(res + offset + sizeof(int), vec[i].data(), vec[i].size());
    offset += len2;
  }
  return res;
}
vector<string> decode_vector(const char* str) {
  int num = *(int*)str;
  int offset = sizeof(int);
  vector<string> res;
  res.resize(num);
  for (int i = 0; i < num; i++){
    int len2 = (*(int*)(str + offset));
    string s(str + offset + sizeof(int), len2 - sizeof(int));
    res[i] = s;
    offset += len2;
  }
  return res;
}
const char* encode_map(const map<string, int>& m) {
  int len = sizeof(int);
  for (auto iter = m.begin(); iter != m.end(); iter++) {
    len += sizeof(int) + iter->first.size() + sizeof(int);
  }
  char* res = new char[len];
  int offset = 0;
  int num = m.size();
  memcpy(res + offset, &num, sizeof(int));
  offset += sizeof(int);
  for (auto iter = m.begin(); iter != m.end(); iter++) {
    int len2 = sizeof(int) + iter->first.size();
    memcpy(res + offset, &len2, sizeof(int));
    memcpy(res + offset + sizeof(int), iter->first.data(), iter->first.size());
    offset += len2;
    memcpy(res + offset, &iter->second, sizeof(int));
    offset += sizeof(int);
  }
  return res;
}
map<string, int> decode_map(const char* str) {
  int num = *(int*)str;
  int offset = sizeof(int);
  map<string, int> res;
  for (int i = 0; i < num; i++) {
    int len2 = *(int*)(str + offset);
    string s(str + offset + sizeof(int), len2 - sizeof(int));
    offset += len2;
    int val = *(int*)(str + offset);
    offset += sizeof(int);
    res.insert(std::pair<string, int>(s, val));
  }
  return res;
}

// GRpcChannel
GRpcChannel::~GRpcChannel() {
  delete []p_node_id_;
  delete []p_data_nodes_;
  delete []p_computation_nodes_;
  delete []p_result_nodes_;
  delete []p_connected_nodes_;
}

ssize_t GRpcChannel::Recv(const char* node_id, const char* id, char* data, uint64_t length, int64_t timeout) 
{
  // return _net_io->recv(node_id, data, get_binary_string(id), timeout); 
  if(nullptr == _net_io){cout << "create io failed!" << endl; return 0;}
  if(nullptr == data){cout << "data is nullptr!" << endl; return 0;}
  return _net_io->recv(node_id, id, data, length, timeout);
}

ssize_t GRpcChannel::Send(const char* node_id, const char* id, const char* data, uint64_t length, 
        int64_t timeout) 
{
  if(nullptr == _net_io){cout << "create io failed!" << endl; return 0;}
  // cout << "GRpcChannel::Send, nodeid:" << node_id << ", msg_id:"  << id  
  //   << ", length:" << length << endl;
  return _net_io->send(node_id, id, data, length, timeout);
}

const char* GRpcChannel::GetDataNodeIDs()
{
  if (p_data_nodes_ == nullptr) {
    p_data_nodes_ = encode_vector(channel_config_->data_nodes_);
  }
  return p_data_nodes_;
}

const char* GRpcChannel::GetComputationNodeIDs()
{
  if (p_computation_nodes_ == nullptr) {
    p_computation_nodes_ = encode_map(channel_config_->compute_nodes_);
  }
  return p_computation_nodes_;
}

const char* GRpcChannel::GetResultNodeIDs()
{
  if (p_result_nodes_ == nullptr) {
    p_result_nodes_ = encode_vector(channel_config_->result_nodes_);
  }
  return p_result_nodes_;
}

const char* GRpcChannel::GetCurrentNodeID()
{
  if (p_node_id_ == nullptr) {
    p_node_id_ = encode_string(self_node_info_.id);
  }
  return p_node_id_;
}

const char* GRpcChannel::GetConnectedNodeIDs()
{
  if (p_connected_nodes_ == nullptr) {
    p_connected_nodes_ = encode_vector(_net_io->get_connected_nodeids());
  }
  return p_connected_nodes_;
}

