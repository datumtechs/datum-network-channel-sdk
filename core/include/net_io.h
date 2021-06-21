#pragma once

#include "connection.h"
#include "config.h"
#include "IChannel.h"
#include <atomic>
#include <condition_variable>
#include <iostream>
#include <iomanip>
#include <map>
#include <mutex>
#include <string>
#include <thread>
#include <vector>
#include <functional>
using namespace std;

/**
 * Users only need to include this one header file.
 * 
 * Provides ViaNetIO
 * 
 * Note:
 * 
 * Only the receiving of the server and the sending of the client are totally supported. 
 * 
 * The receiving of the client and the sending of the server are not totally supported.
 */

/**
 * This is the basic class of Network IO.
 */
class BasicIO {
 public:
  virtual ~BasicIO();

  BasicIO(const NodeInfo &node_info, const vector<ViaInfo>& server_infos, 
    map<string, string>* share_data_map_=nullptr,
    error_callback error_callback=nullptr);

 public:
  /**
   * Initialize the client connection.
   */
  virtual bool init(const string& taskid) = 0;
  virtual ssize_t recv(const string& remote_nodeid, string& data, const string& id, int64_t timeout) = 0;
  virtual ssize_t send(const string& remote_nodeid, const string& data, const string& id, int64_t timeout) = 0;

 public:

   /**
   * close the connections(reserved interface).
   */
  void close();

  const vector<string> get_connected_nodeids() 
  {
    vector<string> vec_conn_nid(via_server_infos_.size());
    for(int i = 0; i < via_server_infos_.size(); i++) 
    {
      vec_conn_nid[i] = via_server_infos_[i].id;
    }
    return vec_conn_nid;
  }

 protected:
  NodeInfo node_info_;
  vector<ViaInfo> via_server_infos_;
  map<string, shared_ptr<ClientConnection>> connection_map;
  map<string, string>* share_data_map_ = nullptr;
  error_callback handler;
};


/**
 * Via Net IO.
 */
class ViaNetIO : public BasicIO {
 public:
  using BasicIO::BasicIO;
  virtual ~ViaNetIO() = default;

  bool init(const string& taskid);
  ssize_t recv(const string& remote_nodeid, string& data, const string& id, int64_t timeout);
  ssize_t send(const string& remote_nodeid, const string& data, const string& id, int64_t timeout);
};

