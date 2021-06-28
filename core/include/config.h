#pragma once

#include <string>
#include <vector>
#include <map>
using namespace std;

#include <rapidjson/document.h>
using rapidjson::Document;

struct Node 
{
  string DESC;
  string NODE_ID;
  string NAME;
  string ADDRESS;
  string VIA;
  public:
    void copy_from(const Node& node) 
    {
      DESC.assign(node.DESC);
      NODE_ID.assign(node.NODE_ID);
      NAME.assign(node.NAME);
      ADDRESS.assign(node.ADDRESS);
      VIA.assign(node.VIA);
    }
};
class DataNodeConfig {
  public:
    vector<Node> P;
    std::string to_string();
};

class ResultNodeConfig {
  public:
    vector<Node> P;
    std::string to_string();
};
class NodeInfoConfig {
  public:
    Node node_;
    std::string to_string();
};

class ComputeNodeConfig {
 public:
  ComputeNodeConfig();
  std::string to_string();
  int GetNodeIndex(const string& node_id);

 public:
  vector<Node> P;
};

struct NodeInfo {
  string id;
  string address;
  string via_address;

  NodeInfo() = default;
  NodeInfo(const string& node_id, const string& addr, const string via_addr) 
    : id(node_id), address(addr), via_address(via_addr){}
};

struct ViaInfo {
  string id;
  string address;
  string via;

  ViaInfo() = default;
  ViaInfo(const string& node_id, const string& addr, const string& via_) 
    : id(node_id), address(addr), via(via_){}
};

enum NODE_TYPE {
  NODE_TYPE_INVALID,
  NODE_TYPE_DATA,
  NODE_TYPE_COMPUTE,
  NODE_TYPE_RESULT
};

class ChannelConfig {
 public:
  ChannelConfig() {}
  ChannelConfig(const string& node_id, const string& config_json);
  ChannelConfig(const string& config_json); // use NODE_ID in config_json
  vector<NODE_TYPE> GetNodeType(const string& node_id);
  const Node& GetNode(const string& node_id);

  void CopyNodeInfo(NodeInfo& node_info, const Node& nodeInfo);
  bool isNodeType(const vector<NODE_TYPE>& vec_node_types, const NODE_TYPE nodeType);
  bool isServer(const string& node_id, const vector<NODE_TYPE>& node_types);
  bool GetServerInfos(vector<ViaInfo>& serverInfos, 
    const string& node_id, const vector<NODE_TYPE>& node_types);

  bool GetClientNodeIds(vector<string>& clientNodeIds, 
    const string& node_id, const vector<NODE_TYPE>& node_types);
  

 private:
  bool load(const string& node_id, const string& config_file);
  bool parse(Document& doc);
  bool parse_node_info(Document& doc, bool pass_via=true);
  bool parse_data(Document& doc);
  bool parse_compute(Document& doc);
  bool parse_result(Document& doc);

 public:
  //ComputeNodeConfig& getMpcConfig() { return mpc; }
  void fmt_print();

 public:
  string node_id_ = ""; // NOT USE AT PRESENT
  bool pass_via_ = "true"; 
  string task_id_ = "";
  NODE_TYPE node_type_ = NODE_TYPE_INVALID;
  map<string, NodeInfoConfig> node_info_config_;
  map<string, string> nodeid_to_via_;
  map<string, string> via_to_address_;
  vector<string> data_nodes_;
  map<string, int> compute_nodes_;
  vector<string> result_nodes_;
  DataNodeConfig data_config_;
  ComputeNodeConfig compute_config_;
  ResultNodeConfig result_config_;
};
