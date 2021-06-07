#pragma once

#include "connection.h"
#include "config.h"
#include "io_channel.h"
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
 * Provides NetIOInterface/SSLNetIO/ParallelNetIO/SSLParallelNetIO
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
  // BasicIO() = default;

  // BasicIO(const NodeInfo &node_id, const vector<ViaInfo>& server_infos, 
  //   map<string, string>* share_data_map_=nullptr,
  //   error_callback error_callback=nullptr);

  BasicIO(const NodeInfo &node_id, const vector<ViaInfo>& server_infos, 
    map<string, string>* share_data_map_=nullptr);

 protected:
  virtual bool init_inner() { return true; }

 public:
  /**
   * init the server and clients.
   */
  bool init();
  /**
   * close the connections.
   */
  void close();

 public:
  ssize_t recv(const string& node_id, string& data, const string& id, int64_t timeout);
  ssize_t send(const string& node_id, const string& data, const string& id, int64_t timeout);

 protected:

  NodeInfo node_info_;
  vector<ViaInfo> via_server_infos_;

 protected:
  map<string, shared_ptr<ClientConnection>> connection_map;
  map<string, string>* share_data_map_;
  string self_nodeid_;
  error_callback handler;
};

/**
 * General Net IO.
 */
class NetIOInterface : public BasicIO {
 public:
  using BasicIO::BasicIO;
  virtual ~NetIOInterface() = default;
};

/**
 * Via Net IO.
 */
class ViaNetIO : public BasicIO {
 public:
  using BasicIO::BasicIO;
  virtual ~ViaNetIO() = default;

 protected:
  bool init_inner() {
    return true;
  }
};

