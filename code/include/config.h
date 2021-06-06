#pragma once

/**
 * @todo
 * This file and its implement will be migrate to the `protocol/` directory.
 */
#include <string>
#include <vector>
#include <map>
using namespace std;

#include <rapidjson/document.h>
using rapidjson::Document;

struct Node {
  string DESC;
  string NODE_ID;
  string NAME;
  string HOST;
  string VIA;
  int PORT;
  public:
    void copy_from(const Node& node) {
      DESC.assign(node.DESC);
      NODE_ID.assign(node.NODE_ID);
      NAME.assign(node.NAME);
      HOST.assign(node.HOST);
      PORT = node.PORT;
      VIA = node.VIA;
    }
};

class _Config {
 public:

};

class DataNodeConfig : public _Config {
  public:
    vector<Node> P;
    std::string to_string();
};

class ResultNodeConfig : public _Config {
  public:
    vector<Node> P;
    std::string to_string();
};
class NodeInfoConfig : public _Config {
  public:
    Node node_;
    std::string to_string();
};

class ComputeNodeConfig : public _Config {
 public:
  ComputeNodeConfig();
  std::string to_string();
  int GetNodeIndex(const string& node_id);

 public:
  int FLOAT_PRECISION_M = 13;
  vector<Node> P;
  // By default, the local ciphertext values are saved in model.
  // Currently, only support it as 3-bit bitmap:[P2 P1 P0]
  // 0: none, all local ciphertext
  // 1: P0
  // 2: P1
  // 4: P2
  // 3: P0 and P1
  // 5: P0 and P2
  // 6: P1 and P2
  // 7: P0, P1 and P2
  int SAVER_MODE = 0;

  /**
   * for general 3PC
   *  b000(0), cipher model, each node_id has the secret sharing value of the model.
   *  b001/b010/b100, plain model, only P0(b001) or P1(b010) or P2(b100) owns the plain model, load as private;
   *  b111(-1), plain model, all parties have the plain model, load as public-constant;
   *  others, not supported
   * you can set RESTORE_MODE to 0~7 or -1.
   */
  int RESTORE_MODE = 0;

  // server and client certifications
  string SERVER_CERT = "certs/server-nopass.cert";
  string SERVER_PRIKEY = "certs/server-prikey";
  string SERVER_PRIKEY_PASSWORD = "123456";
};

struct NodeInfo {
  string id;
  string address;
  int port;

  NodeInfo() = default;
  NodeInfo(const string& node_id, const string& addr, int _port) 
    : id(node_id), address(addr), port(_port){}
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
  NODE_TYPE node_type_ = NODE_TYPE_INVALID;
  map<string, NodeInfoConfig> node_info_config_;
  map<string, string> nodeid_to_via_;
  map<string, string> via_to_info_;
  vector<string> data_nodes_;
  map<int, string> compute_nodes_;
  vector<string> result_nodes_;
  DataNodeConfig data_config_;
  ComputeNodeConfig compute_config_;
  ResultNodeConfig result_config_;
};
