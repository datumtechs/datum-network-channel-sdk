#include "config.h"

#include <rapidjson/document.h>
#include <rapidjson/prettywriter.h>
#include <rapidjson/stringbuffer.h>
#include <rapidjson/writer.h>
using namespace rapidjson;

#include <algorithm>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <set>
using namespace std;

#include <unistd.h>

bool is_file_exist(const string& filepath) {
  if (filepath.empty())
    return false;
  return (access(filepath.c_str(), F_OK) == 0);
}
void if_key_not_exist_then_exit(bool must_exist, const char* key) {
  if (must_exist) {
    throw "key[" + string(key) + "] not exist!";
  }
}
std::string GetString(
  rapidjson::Value& v,
  const char* key,
  const char* default_value = "",
  bool must_exist = false) {
  if (v.HasMember(key)) {
    return v[key].GetString();
  }
  if_key_not_exist_then_exit(must_exist, key);
  return std::string(default_value);
}
int GetInt(rapidjson::Value& v, const char* key, int default_value = 0, bool must_exist = false) {
  if (v.HasMember(key)) {
    return v[key].GetInt();
  }
  if_key_not_exist_then_exit(must_exist, key);
  return default_value;
}
float GetFloat(
  rapidjson::Value& v,
  const char* key,
  float default_value = 0.0f,
  bool must_exist = false) {
  if (v.HasMember(key)) {
    return v[key].GetFloat();
  }
  if_key_not_exist_then_exit(must_exist, key);
  return default_value;
}
bool GetBool(
  rapidjson::Value& v,
  const char* key,
  bool default_value = 0,
  bool must_exist = false) {
  if (v.HasMember(key)) {
    return v[key].GetBool();
  }
  if_key_not_exist_then_exit(must_exist, key);
  return default_value;
}

ComputeNodeConfig::ComputeNodeConfig() {}

std::string ComputeNodeConfig::to_string() {
  std::stringstream sss;

  for (int i = 0; i < P.size(); i++) {
    sss << "\n        P" << i << " NAME: " << P[i].NAME;
    sss << "\n        P" << i << " ADDRESS: " << P[i].ADDRESS;
  }
  sss << "\n";
  return sss.str();
}

int ComputeNodeConfig::GetNodeIndex(const string& node_id) {
  for (int i = 0; i < P.size(); i++) {
    if (node_id == P[i].NODE_ID)
      return i;
  }
  return -1;
}

std::string ResultNodeConfig::to_string() {
  std::stringstream sss;

  for (int i = 0; i < P.size(); i++) {
    sss << "\n        P" << i << " NAME: " << P[i].NAME;
    sss << "\n        P" << i << " ADDRESS: " << P[i].ADDRESS;
  }
  sss << "\n";
  return sss.str();
}

std::string DataNodeConfig::to_string() {
  std::stringstream sss;

  for (int i = 0; i < P.size(); i++) {
    sss << "\n        P" << i << " NAME: " << P[i].NAME;
    sss << "\n        P" << i << " ADDRESS: " << P[i].ADDRESS;
  }
  sss << "\n";
  return sss.str();
}

ChannelConfig::ChannelConfig(const string& node_id, const string& config_json) {
  //! @attention use node_id__ID = node_id
  bool ret = load(node_id, config_json);
  if (!ret) {
    throw "ChannelConfig load2 config json failed!";
  }
  node_id_ = node_id;
}
ChannelConfig::ChannelConfig(const string& config_json) {
  //! @attention use node_id__ID in config_json
  const string node_id = "";
  bool ret = load(node_id, config_json);
  if (!ret) 
  {
    throw "ChannelConfig load3 config json failed!";
  }
}

vector<NODE_TYPE> ChannelConfig::GetNodeType(const string& node_id) {
  vector<NODE_TYPE> node_types;
  for (int i = 0; i < data_config_.P.size(); i++) {
    if (node_id == data_config_.P[i].NODE_ID)
      node_types.push_back(NODE_TYPE_DATA);
  }

  for (int i = 0; i < compute_config_.P.size(); i++) {
    if (node_id == compute_config_.P[i].NODE_ID)
      node_types.push_back(NODE_TYPE_COMPUTE);
  }

  for (int i = 0; i < result_config_.P.size(); i++) {
    if (node_id == result_config_.P[i].NODE_ID)
      node_types.push_back(NODE_TYPE_RESULT);
  }

  return node_types;
}

const Node& ChannelConfig::GetNode(const string& node_id) {
  for (int i = 0; i < data_config_.P.size(); i++) {
    if (node_id == data_config_.P[i].NODE_ID)
      return data_config_.P[i];
  //  cout << "data node id:" << data_config_.P[i].NODE_ID << endl;
  }

  for (int i = 0; i < compute_config_.P.size(); i++) {
    if (node_id == compute_config_.P[i].NODE_ID)
      return compute_config_.P[i];
  //  cout << "compute node id:" << compute_config_.P[i].NODE_ID << endl;
  }

  for (int i = 0; i < result_config_.P.size(); i++) {
    if (node_id == result_config_.P[i].NODE_ID)
      return result_config_.P[i];
  //  cout << "result node id:" << result_config_.P[i].NODE_ID << endl;
  }
  cout << "node_id: " << node_id << endl;
  throw "can not find node in config!";
}

bool ChannelConfig::load(const string& node_id, const string& config_file) {
  node_id_ = node_id;
  // config_json: json-file or json-string

  string sjson(config_file);
  ifstream ifile(config_file);
  if (!ifile.is_open()) {
    //log_warn << "open " << config_file << " error!\n";
    cout << "try to load as json string" << endl;
  } else {
    sjson = "";
    while (!ifile.eof()) {
      string s;
      getline(ifile, s);
      sjson += s;
    }
    ifile.close();
  }

  Document doc;
  if (doc.Parse(sjson.data()).HasParseError()) {
    cout << "parser " << config_file << " error!\n";
    return false;
  }

  StringBuffer buffer;
  PrettyWriter<StringBuffer> writer(buffer);
  doc.Accept(writer);
  string data = buffer.GetString();
  cout << "Config Source String:\n" << data << endl;

  if (!parse(doc)) {
    cout << "parse error" << endl;
    return false;
  }

  return true;
}

bool ChannelConfig::parse_node_info(Document& doc, bool pass_via) 
{
  if (doc.HasMember("NODE_INFO") && doc["NODE_INFO"].IsArray()) 
  {
    Value& Nodes = doc["NODE_INFO"];

    // nodes
    for (int i = 0; i < Nodes.Size(); i++) 
    {
      NodeInfoConfig cfg;
      Value& Node = Nodes[i];
      cfg.node_.NODE_ID = GetString(Node, "NODE_ID", "", false);
      cout << "node info parse:" << cfg.node_.NODE_ID << endl;
      cfg.node_.NAME = GetString(Node, "NAME", (std::string("P") + std::to_string(i)).c_str(), false);
      cfg.node_.ADDRESS = GetString(Node, "ADDRESS", "", false);
      cfg.node_.VIA = GetString(Node, "VIA", "", false);
      node_info_config_.insert(std::pair<string, NodeInfoConfig>(cfg.node_.NODE_ID, cfg));
      
      if(pass_via_) 
      {
        nodeid_to_via_.insert(std::pair<string, string>(cfg.node_.NODE_ID, cfg.node_.VIA));
      }
    }
    // cout << "parse " << Nodes.Size() << " node info success" << endl;

    if (pass_via && doc.HasMember("VIA_INFO") && doc["VIA_INFO"].IsObject()) 
    {
      Value& Vias = doc["VIA_INFO"];

      for (auto iter = Vias.MemberBegin(); iter != Vias.MemberEnd(); iter++) 
      {
        string name = iter->name.GetString();
        string value = iter->value.GetString();
        via_to_address_.insert(std::pair<string, string>(name, value));
      }

    //  cout << "parse via info success" << endl;
    }
  }
  return true;
}

bool ChannelConfig::parse_data(Document& doc) {
  if (doc.HasMember("DATA_NODES") && doc["DATA_NODES"].IsArray()) {
    Value& Nodes = doc["DATA_NODES"];

    // nodes
    data_nodes_.resize(Nodes.Size());
    data_config_.P.resize(Nodes.Size());
    for (int i = 0; i < Nodes.Size(); i++) {
      Value& Node = Nodes[i];
      data_nodes_[i] = Node.GetString();
      if (node_info_config_.find(data_nodes_[i]) != node_info_config_.end()) {
        data_config_.P[i].copy_from(node_info_config_[data_nodes_[i]].node_);
      } else {
        cout << "can not find node info, node id:" << data_nodes_[i] << endl;
      }
    }
    cout << "parse " << Nodes.Size() << " data success" << endl;

  }
  return true;
}

bool ChannelConfig::parse_compute(Document& doc) {
  if (doc.HasMember("COMPUTATION_NODES") && doc["COMPUTATION_NODES"].IsObject()) {
    Value& Nodes = doc["COMPUTATION_NODES"];

    // nodes
    for (auto iter = Nodes.MemberBegin(); iter != Nodes.MemberEnd(); iter++) {
      string name = iter->name.GetString();
      string value = iter->value.GetString();
      int party = -1;
      if (value == "P0") {
        party = 0;
      } else if (value == "P1") {
        party = 1;
      } else if (value == "P2") {
        party = 2;
      }
      compute_nodes_.insert(std::pair<string, int>(name, party));
    }

    compute_config_.P.resize(compute_nodes_.size());
    int i = 0;
    for (auto iter = compute_nodes_.begin(); iter != compute_nodes_.end(); iter++, i++) {
      if (node_info_config_.find(iter->first) != node_info_config_.end()) {
        compute_config_.P[i].copy_from(node_info_config_[iter->first].node_);
      } else {
        cout << "can not find node info, node id:" << iter->first << endl;
      }
    }
    cout << "parse " << " computation success" << endl;
  }
  return true;
}

bool ChannelConfig::parse_result(Document& doc) {
  if (doc.HasMember("RESULT_NODES") && doc["RESULT_NODES"].IsArray()) {
    Value& Nodes = doc["RESULT_NODES"];

    // nodes
    result_nodes_.resize(Nodes.Size());
    result_config_.P.resize(Nodes.Size());
    for (int i = 0; i < Nodes.Size(); i++) {
      Value& Node = Nodes[i];
      result_nodes_[i] = Node.GetString();
      if (node_info_config_.find(result_nodes_[i]) != node_info_config_.end()) {
        result_config_.P[i].copy_from(node_info_config_[result_nodes_[i]].node_);
      } else {
        cout << "can not find node info, node id:" << result_nodes_[i] << endl;
      }
    }
    cout << "parse " << Nodes.Size() << " result success" << endl;
  }
  return true;
}

bool ChannelConfig::parse(Document& doc) {
  //! @todo the node_id__ID field in CONFIG.json have not yet used
  // pass_via_ = GetBool(doc, "PASS_VIA", true, false);
  task_id_ = GetString(doc, "TASK_ID", "", false);
  if (!parse_node_info(doc)) {
    cout << "parse node info error" << endl;
  }

  if (!parse_data(doc)) {
    cout << "parse data error" << endl;
    return false;
  }

  if (!parse_compute(doc)) {
    cout << "parse compute error" << endl;
    return false;
  }

  if (!parse_result(doc)) {
    cout << "parse result error" << endl;
    return false;
  }

  // fmt_print();
  return true;
}

void ChannelConfig::fmt_print() {
  cout << "=======================================" << endl;
  cout << "          node_id_: " << node_id_ << endl;
  cout << data_config_.to_string();
  cout << compute_config_.to_string();
  cout << result_config_.to_string();
  cout << "=======================================" << endl;
}

void ChannelConfig::CopyNodeInfo(NodeInfo& node_info, const Node& nodeInfo) 
{
  node_info.id = nodeInfo.NODE_ID;
  node_info.address = nodeInfo.ADDRESS;
}

bool ChannelConfig::isNodeType(const vector<NODE_TYPE>& vec_node_types, const NODE_TYPE nodeType)
{
  for(int i=0; i < vec_node_types.size(); ++i)
  {
    if(vec_node_types[i] == nodeType)
    {
      return true;
    }
  }
  return false;
}

bool ChannelConfig::GetNodeInfos(vector<string>& clientNodeIds, vector<ViaInfo>& serverInfos, 
    const string& node_id)
{
  set<string> nodeid_set;
  // 遍历计算节点
  for (int i = 0; i < data_config_.P.size(); i++) 
  {
    // 获取计算节点的nodeid
    string nid = data_config_.P[i].NODE_ID;
    string via = nodeid_to_via_[nid];
    if (node_id != nid && nodeid_set.find(nid) == nodeid_set.end()) 
    {
      ViaInfo viaTmp;
      viaTmp.id = nid;
      viaTmp.via = via;
      viaTmp.address = via_to_address_[via];
      // cout << "id: " << nid << ", via: " << viaTmp.via << ", address: " << viaTmp.address << endl;
      serverInfos.push_back(viaTmp);
      clientNodeIds.push_back(nid);
      nodeid_set.insert(nid);
    }
  }

  // 遍历计算节点
  for (int i = 0; i < compute_config_.P.size(); i++) 
  {
    // 获取计算节点的nodeid
    string nid = compute_config_.P[i].NODE_ID;
    string via = nodeid_to_via_[nid];
    if (node_id != nid && nodeid_set.find(nid) == nodeid_set.end()) 
    {
      ViaInfo viaTmp;
      viaTmp.id = nid;
      viaTmp.via = via;
      viaTmp.address = via_to_address_[via];
      // cout << "id: " << nid << ", via: " << viaTmp.via << ", address: " << viaTmp.address << endl;
      serverInfos.push_back(viaTmp);
      clientNodeIds.push_back(nid);
      nodeid_set.insert(nid);
    }
  }
  
  for (int i = 0; i < result_config_.P.size(); i++) 
  {
    // cout << "handle compute node" << endl;
    string nid = result_config_.P[i].NODE_ID;
    string via = nodeid_to_via_[nid];
    if (node_id != nid && nodeid_set.find(nid) == nodeid_set.end()) 
    {
      ViaInfo viaTmp;
      viaTmp.id = nid;
      // 节点所在via
      viaTmp.via = via;
      // via信息
      viaTmp.address = via_to_address_[viaTmp.via];
      // cout << "id: " << nid << ", via: " << viaTmp.via << ", address: " << viaTmp.address << endl;
      serverInfos.push_back(viaTmp);
      clientNodeIds.push_back(nid);
      nodeid_set.insert(nid);
    }
  }  
  return true;
}
