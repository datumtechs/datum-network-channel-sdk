#pragma once
#include <memory>
#include <string>
#include <vector>
#include <map>
#include <functional>
#include <cstdint>
using namespace std;

typedef std::function<void(const char*, const char*, int, const char*, void*)> error_callback;
const char* encode_string(const string& str);
string decode_string(const char* str);
const char* encode_vector(const vector<string>& vec);
vector<string> decode_vector(const char* str);
const char* encode_map(const map<string, int>& m);
map<string, int> decode_map(const char* str);

/// Channel interface definition
/// the functionality is sending a message to peer and 
/// receiving a message from peer
class IChannel {
public:
  virtual ~IChannel() {};

   /**
   * @brief Set error callback for error handle.
   * @param error_cb error callback to handle error.
   * @note Should set callback from python to c++, Rosetta internal should not set error callback.
  */
  virtual void SetErrorCallback(error_callback error_cb)= 0;

  /**
   * @brief Recv receive a message from message queue， for the target node (blocking for timeout millisecond, default waiting forever)
   * @param node_id target node id for message receiving.
   * @param id identity of a message, could be a task id or message id.
   * @param data buffer to receive a message.
   * @param length data length expect to receive
   * @param timeout timeout to receive a message(Unit: milliseconds).
   * @return 
   *  return message length if receive a message successfully
   *  0 if peer is disconnected  
   *  -1 if it gets a exception or error
  */
  virtual int64_t Recv(const char* node_id, const char* id, char* data, uint64_t length, int64_t timeout=-1) = 0;

  /**
   * @brief Send send a message to target node
   * @param node_id target node id for message receiving
   * @param id identity of a message, could be a task id or message id.
   * @param data buffer to send
   * @param length data length expect to send
   * @param timeout timeout to receive a message(Unit: milliseconds).
   * @return 
   *  return length of data has been sent if send a message successfully
   *  -1 if gets exceptions or error
  */
  virtual int64_t Send(const char* node_id, const char* id, const char* data, uint64_t length, int64_t timeout=-1) = 0;

  /**
   * @brief get node id of all the data nodes
   * @return
   * return node id of all the data nodes
  */
  virtual const char* GetDataNodeIDs() = 0;

  /**
   * @brief get node id and party id of all the computation nodes
   * @return
   * return node id and party id of all the computation nodes
   * string  indicates node id and int indicates party id
  */
  virtual const char* GetComputationNodeIDs() = 0;

  /**
   * @brief get node id of all the result nodes
   * @return
   * return node id of all the result nodes
  */
  virtual const char* GetResultNodeIDs() = 0;
  /**
   * @brief get node id of the current node
   * @return
   * return node id of the current node
  */
  virtual const char* GetCurrentNodeID() = 0;

  /**
   * @brief get node id of all the nodes establishing connection with the current node
   * @return
   * return node id of all the nodes establishing connection with the current node
  */
  virtual const char* GetConnectedNodeIDs() = 0;

  // virtual const char* GetCurrentVia() = 0;
  // virtual const char* GetCurrentAddress() = 0;
  // virtual const char* GetTaskId() = 0;
};// IChannel

shared_ptr<IChannel> CreateChannel(const string& node_id, const string &config_str, 
      error_callback error_cb);

void set_channel(shared_ptr<IChannel> io_channel);
string recv_msg(const string& node_id, const string& msg_id, uint64_t msg_len); 
void send_msg(const string& node_id, const string& msg_id, const string& data, uint64_t msg_len); 
