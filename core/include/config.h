#pragma once
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <map>
#include <iostream>
#include "const.h"
#include <set>
using namespace std;

#include <rapidjson/document.h>
using rapidjson::Document;
#define RET_SUCCEED_CODE 0
struct Node 
{
  string DESC;
  string NODE_ID;
  string NAME;
  string ADDRESS;
  string PUBLIC_IP;
  string VIA;
  string GRICER2;
  string ICEGRID;
  string CERT_DIR;
  string CA_CERT_PATH;
  string SERVER_KEY_PATH;
  string SERVER_CERT_PATH;
  string CLIENT_KEY_PATH;
  string CLIENT_CERT_PATH;
  string PASSWORD;
#if(2 == SSL_TYPE)
  string SERVER_SIGN_KEY_PATH;
  string SERVER_SIGN_CERT_PATH;
  string SERVER_ENC_KEY_PATH;
  string SERVER_ENC_CERT_PATH;
  string CLIENT_SIGN_KEY_PATH;
  string CLIENT_SIGN_CERT_PATH;
  string CLIENT_ENC_KEY_PATH;
  string CLIENT_ENC_CERT_PATH;
#endif

  public:
    void copy_from(const Node& node) 
    {
      DESC.assign(node.DESC);
      NODE_ID.assign(node.NODE_ID);
      NAME.assign(node.NAME);
      ADDRESS.assign(node.ADDRESS);
      PUBLIC_IP.assign(node.PUBLIC_IP);
      VIA.assign(node.VIA);
      GRICER2.assign(node.GRICER2);
      ICEGRID.assign(node.ICEGRID);
      CERT_DIR.assign(node.CERT_DIR);
      CA_CERT_PATH.assign(node.CA_CERT_PATH);

      SERVER_KEY_PATH.assign(node.SERVER_KEY_PATH);
      SERVER_CERT_PATH.assign(node.SERVER_CERT_PATH);
      CLIENT_KEY_PATH.assign(node.CLIENT_KEY_PATH);
      CLIENT_CERT_PATH.assign(node.CLIENT_CERT_PATH); 
      PASSWORD.assign(node.PASSWORD); 
      #if(2 == SSL_TYPE)
      {
        SERVER_SIGN_KEY_PATH.assign(node.SERVER_SIGN_KEY_PATH);
        SERVER_SIGN_CERT_PATH.assign(node.SERVER_SIGN_CERT_PATH);
        SERVER_ENC_KEY_PATH.assign(node.SERVER_ENC_KEY_PATH);
        SERVER_ENC_CERT_PATH.assign(node.SERVER_ENC_CERT_PATH);
        CLIENT_SIGN_KEY_PATH.assign(node.CLIENT_SIGN_KEY_PATH);
        CLIENT_SIGN_CERT_PATH.assign(node.CLIENT_SIGN_CERT_PATH);
        CLIENT_ENC_KEY_PATH.assign(node.CLIENT_ENC_KEY_PATH);
        CLIENT_ENC_CERT_PATH.assign(node.CLIENT_ENC_CERT_PATH);
      } 
      #endif
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

// ICE插件配置结构体
struct IcePlugCfg {
  string AppName_;
  string Ip_;
  string Port_;

  IcePlugCfg() = default;
  IcePlugCfg(const string& AppName, const string& Ip, const string& Port):
    AppName_(AppName), Ip_(Ip), Port_(Port){}
  
  const string GetInfo() const
  {
    return "AppName:" + AppName_ + ", Address:" + Ip_ + ":" + Port_;
  }
};

struct NodeInfo {
  string id;
  string address;
  string public_ip_;
  string via_address;
  IcePlugCfg glacier2_info;
  IcePlugCfg ice_grid_info;
  string ca_cert_path_;
  string cert_dir_;

  string server_key_path_;
  string server_cert_path_;
  // Used as a client certificate to invoke the VIA server registration interface
  string client_key_path_;
  string client_cert_path_;
  string password_;
#if(2==SSL_TYPE) 
  string server_sign_key_path_;
  string server_sign_cert_path_;
  string server_enc_key_path_;
  string server_enc_cert_path_;
  // Used as a client certificate to invoke the VIA server registration interface
  string client_sign_key_path_;
  string client_sign_cert_path_;
  string client_enc_key_path_;
  string client_enc_cert_path_;
#endif

  NodeInfo() = default;
  NodeInfo(const string& node_id, const string& addr, const string& via_addr,
    const string& glacier2_addr, const string& ice_grid_addr): id(node_id), address(addr), 
      via_address(via_addr),glacier2_info(), ice_grid_info(){}
};

struct ViaInfo {
  string id;
  string via_address;
  string via;
   // 客户端连接glacier2信息
  IcePlugCfg glacier2_info;
  string server_cert_path_;
  string ca_cert_path_;
  string cert_dir_;

  string client_key_path_;
  string client_cert_path_;
  string password_;
  #if(2==SSL_TYPE)  
    string client_sign_key_path_;
    string client_sign_cert_path_;
    string client_enc_key_path_;
    string client_enc_cert_path_;
  #endif

  ViaInfo() = default;
  ViaInfo(const string& node_id, const string& via_addr, const string& via_, const string& glacier2_addr) 
    : id(node_id), via_address(via_addr), via(via_), glacier2_info(){}
  friend bool operator<(const ViaInfo& v1, const ViaInfo& v2) {
    return v1.id < v2.id;
  }
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
  bool GetNodeInfos(set<string>& clientNodeIds, set<ViaInfo>& serverInfos, const string& node_id);
  
 private:
  bool load(const string& node_id, const string& config_file);
  bool parse(Document& doc);
  bool parse_node_info(Document& doc);
  bool parse_data(Document& doc);
  bool parse_compute(Document& doc);
  bool parse_result(Document& doc);
  bool parse_policy(Document& doc);
  bool GetAllNodeInfos(set<string>& clientNodeIds, set<ViaInfo>& serverInfos, const string& node_id);
  bool GetInfoByNodeId(set<string>& clientNodeIds, set<ViaInfo>& serverInfos, const string& node_id);

 public:
  void fmt_print();

 public:
  string node_id_ = ""; // NOT USE AT PRESENT
  string task_id_ = "";
  string root_cert_ = "";
  uint8_t log_level_ = 0; // 0:Debug, 1:Info, 2: Error
  float ping_time_ = 1.0; // Interval (unit: s) for ping the server when the client is connected to the server.  
  float send_timeout_ = 5.0;  
  float conn_timeout_ = 5.0;
  int32_t buffer_size_ = 1024 * 8;  
 
  NODE_TYPE node_type_ = NODE_TYPE_INVALID;
  map<string, NodeInfoConfig> node_info_config_;
  map<string, string> nodeid_to_via_;
  map<string, string> via_to_address_;
  // ice config
  map<string, string> nodeid_to_glacier2_;
  map<string, string> nodeid_to_icegrid_;
  map<string, IcePlugCfg> glacier2_to_info_;
  map<string, IcePlugCfg> icegrid_to_info_;
  vector<string> data_nodes_;
  map<string, int> compute_nodes_;
  vector<string> result_nodes_;
  set<string> task_nodes_;
  DataNodeConfig data_config_;
  ComputeNodeConfig compute_config_;
  ResultNodeConfig result_config_;
  // connect policy
  string policy_type_ = "";
  map<string, set<string>> map_policy_;
};

static string get_file_contents(const string& fpath)
{
    ifstream ifile(fpath);
    if(!ifile.good())
    {
        cerr << "file is not exist:" << fpath << endl;
        return "";
    }
    ostringstream buf;
    char ch;
    while(buf&&ifile.get(ch))
    buf.put(ch);
    return buf.str();
}
