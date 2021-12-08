#include "grpc_channel.h"
#include <mutex>
using namespace std;
map<IChannel*, const char*> g_current_node_map;
map<IChannel*, const NodeIDVec*> g_data_node_map;
map<IChannel*, const NodeIDMap*> g_computation_node_map;
map<IChannel*, const NodeIDVec*> g_result_node_map;
map<IChannel*, const NodeIDVec*> g_connected_node_map;
std::mutex g_current_node_mutex;
std::mutex g_data_node_mutex;
std::mutex g_computation_node_mutex;
std::mutex g_result_node_mutex;
std::mutex g_connected_node_mutex;

/**
 * @brief encode c++ standard string to C string
 * @param str c++ standard string
 * @return C string
*/
const char* encode_string(const string& str) 
{
  char* res = new char[str.size() + 1];
  memset(res, 0, str.size() + 1);
  memcpy(res, str.data(), str.size());
  return res;
}

/**
 * @brief encode c++ vector<string> to NodeIDVec
 * @param vec data to be encoded
 * @return a pointer to NodeIDVec
*/
const NodeIDVec* encode_vector(const vector<string>& vec) 
{
  NodeIDVec* res = new NodeIDVec();
  res->node_count = vec.size();
  res->node_ids = new char*[vec.size()];
  for (int i = 0; i < vec.size(); i++) {
    res->node_ids[i] = new char[vec[i].size() + 1];
    memset(res->node_ids[i], 0, vec[i].size() + 1);
    memcpy(res->node_ids[i], vec[i].data(), vec[i].size());
  }
  return res;
}

/**
 * @brief encode C++ map<string,int> to NodeIDMap
 * @param m data to be encoded
 * @return a pointer to NodeIDMap
*/
const NodeIDMap* encode_map(const map<string, int>& m) 
{
  NodeIDMap* pointer = new NodeIDMap();
  pointer->node_count = m.size();
  pointer->pairs = new NodeIDPair*[m.size()];
  int i = 0;
  for (auto iter = m.begin(); iter != m.end(); iter++) {
    pointer->pairs[i] = new NodeIDPair();
    pointer->pairs[i]->node_id = new char[iter->first.size() + 1];
    memset(pointer->pairs[i]->node_id, 0, iter->first.size() + 1);
    memcpy(pointer->pairs[i]->node_id, iter->first.data(), iter->first.size());
    pointer->pairs[i]->party_id = iter->second;
    i++;
  }
  return pointer;
}

// GRpcChannel
GRpcChannel::~GRpcChannel() 
{
  DestroyCurrentNode();
  DestroyDataNodes();
  DestroyComputationNodes();
  DestroyResultNodes();
  DestroyConnectedNodes();
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
  cout << "GRpcChannel::Send, nodeid:" << node_id << ", msg_id:"  << id  
    << ", length:" << length << endl;
  return _net_io->send(node_id, id, data, length, timeout);
}

/**
 * @brief get node id of all the data nodes
 * @param channel Channel Handler
 * @return
 * return node id of all the data nodes
*/
const NodeIDVec* GRpcChannel::GetDataNodeIDs() {
  std::unique_lock<std::mutex> lck(g_data_node_mutex);
  auto iter = g_data_node_map.find(this);
  if ( iter == g_data_node_map.end()) {
    const NodeIDVec* nodes = encode_vector(getDataNodeIDs());
    g_data_node_map.insert(std::pair<IChannel*, const NodeIDVec*>(this, nodes));
    return nodes;
  }
  return iter->second;
}

/**
 * @brief get node id and party id of all the computation nodes
 * @param channel Channel Handler
 * @return
 * return node id and party id of all the computation nodes
 * string  indicates node id and int indicates party id
*/
const NodeIDMap* GRpcChannel::GetComputationNodeIDs() {
  std::unique_lock<std::mutex> lck(g_computation_node_mutex);
  auto iter = g_computation_node_map.find(this);
  if ( iter == g_computation_node_map.end()) {
    const NodeIDMap* nodes = encode_map(getComputationNodeIDs());
    g_computation_node_map.insert(std::pair<IChannel*, const NodeIDMap*>(this, nodes));
    return nodes;
  }
  return iter->second;
}

/**
 * @brief get node id of all the result nodes
 * @param channel Channel Handler
 * @return
 * return node id of all the result nodes
*/
const NodeIDVec* GRpcChannel::GetResultNodeIDs() {
  std::unique_lock<std::mutex> lck(g_result_node_mutex);
  auto iter = g_result_node_map.find(this);
  if ( iter == g_result_node_map.end()) {
    const NodeIDVec* nodes = encode_vector(getResultNodeIDs());
    g_result_node_map.insert(std::pair<IChannel*, const NodeIDVec*>(this, nodes));
    return nodes;
  }
  return iter->second;
}
/**
 * @brief get node id of the current node
 * @param channel Channel Handler
 * @return
 * return node id of the current node
*/
const char* GRpcChannel::GetCurrentNodeID() 
{
  std::unique_lock<std::mutex> lck(g_current_node_mutex);
  auto iter = g_current_node_map.find(this);
  if ( iter == g_current_node_map.end()) {
    const char* nodes = encode_string(getCurrentNodeID());
    g_current_node_map.insert(std::pair<IChannel*, const char*>(this, nodes));
    return nodes;
  }
  return iter->second;
}

/**
 * @brief get node id of all the nodes establishing connection with the current node
 * @param channel Channel Handler
 * @return
 * return node id of all the nodes establishing connection with the current node
*/
const NodeIDVec* GRpcChannel::GetConnectedNodeIDs() 
{
  std::unique_lock<std::mutex> lck(g_connected_node_mutex);
  auto iter = g_connected_node_map.find(this);
  if ( iter == g_connected_node_map.end()) {
    const NodeIDVec* nodes = encode_vector(getConnectedNodeIDs());
    g_connected_node_map.insert(std::pair<IChannel*, const NodeIDVec*>(this, nodes));
    return nodes;
  }
  return iter->second;
}


const vector<string>& GRpcChannel::getDataNodeIDs() 
{
  return channel_config_->data_nodes_;
}

const map<string, int>& GRpcChannel::getComputationNodeIDs() 
{
  return channel_config_->compute_nodes_;
}

const vector<string>& GRpcChannel::getResultNodeIDs() 
{
  return channel_config_->result_nodes_;
}

const string& GRpcChannel::getCurrentNodeID()
{
  return self_node_info_.id;
}

const vector<string>& GRpcChannel::getConnectedNodeIDs() 
{
  return connected_nodes_;
}

void GRpcChannel::DestroyCurrentNode() 
{
  std::unique_lock<std::mutex> lck(g_current_node_mutex);
  auto iter = g_current_node_map.find(this);
  if (iter != g_current_node_map.end()) {
    delete []iter->second;
    g_current_node_map.erase(iter);
  }
}

void GRpcChannel::DestroyDataNodes() 
{
  std::unique_lock<std::mutex> lck(g_data_node_mutex);
  auto iter = g_data_node_map.find(this);
  if (iter != g_data_node_map.end()) {
    for (int i = 0; i < iter->second->node_count; i++) {
      delete []iter->second->node_ids[i];
    }
    delete []iter->second->node_ids;
    delete iter->second;
    g_data_node_map.erase(iter);
  }
}

void GRpcChannel::DestroyComputationNodes() 
{
  std::unique_lock<std::mutex> lck(g_computation_node_mutex);
  auto iter = g_computation_node_map.find(this);
  if (iter != g_computation_node_map.end()) {
    for (int i = 0; i < iter->second->node_count; i++) {
      delete []iter->second->pairs[i]->node_id;
      delete iter->second->pairs[i];
    }
    delete []iter->second->pairs;
    delete iter->second;
    g_computation_node_map.erase(iter);
  }
}

void GRpcChannel::DestroyResultNodes() 
{
  std::unique_lock<std::mutex> lck(g_result_node_mutex);
  auto iter = g_result_node_map.find(this);
  if (iter != g_result_node_map.end()) {
    for (int i = 0; i < iter->second->node_count; i++) {
      delete []iter->second->node_ids[i];
    }
    delete []iter->second->node_ids;
    delete iter->second;
    g_result_node_map.erase(iter);
  }
}

void GRpcChannel::DestroyConnectedNodes() 
{
  std::unique_lock<std::mutex> lck(g_connected_node_mutex);
  auto iter = g_connected_node_map.find(this);
  if (iter != g_connected_node_map.end()) {
    for (int i = 0; i < iter->second->node_count; i++) {
      delete []iter->second->node_ids[i];
    }
    delete []iter->second->node_ids;
    delete iter->second;
    g_connected_node_map.erase(iter);
  }
}
