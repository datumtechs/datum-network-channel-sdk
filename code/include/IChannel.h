#pragma once

#include <string>
#include <vector>
#include <map>
#include <functional>
#include <cstdint>
using namespace std;


typedef std::function<void(const string& node_id, const string& id, int errorno, 
  const string& errormsg, void* user_data)> error_callback;


/// Channel interface definition
/// the functionality is sending a message to peer and 
/// receiving a message from peer
class IChannel {
public:
  IChannel()=default;
  virtual ~IChannel()=default;

   /**
   * @brief Set error callback for error handle.
   * @param error_cb error callback to handle error.
   * @note Should set callback from python to c++, Rosetta internal should not set error callback.
  */
  virtual void SetErrorCallback(error_callback error_cb)= 0;

  /**
   * @brief RecvMessage receive a message from message queue， for the target node (blocking for timeout microseconds, default waiting forever)
   * @param party_id target node id for message receiving.
   * @param id identity of a message, could be a task id or message id.
   * @param data buffer to receive a message.
   * @param timeout timeout to receive a message(microseconds).
   * @return 
   *  return message length if receive a message successfully
   *  0 if peer is disconnected  
   *  -1 if it gets a exception or error
   * if SetMessageHandler is called, then RecvMessage is desired to be disabled.
  */
  virtual int64_t Recv(const string& node_id, const std::string& id, std::string& data, int64_t timeout=-1) = 0;

  /**
   * @brief SendMessage send a message to target node
   * @param party_id target node id for message receiving
   * @param id identity of a message, could be a task id or message id.
   * @param data buffer to receive a message(microseconds)
   * @return 
   *  return 0 if send a message successfully
   *  -1 if gets exceptions or error
  */
  virtual int64_t Send(const string& node_id, const std::string& id,const std::string& data, int64_t timeout=-1) = 0;

  /**
   * @brief get node id of all the data nodes
   * @return
   * return node id of all the data nodes
  */
  virtual vector<string> GetDataNodeIDs() = 0;

  /**
   * @brief get node id and party id of all the computation nodes
   * @return
   * return node id and party id of all the computation nodes
   * string  indicates node id and int indicates party id
  */
  virtual map<string, int> GetComputationNodeIDs() = 0;

  /**
   * @brief get node id of all the result nodes
   * @return
   * return node id of all the result nodes
  */
  virtual vector<string> GetResultNodeIDs() = 0;
  /**
   * @brief get node id of the current node
   * @return
   * return node id of the current node
  */
  virtual string GetCurrentNodeID() = 0;

  /**
   * @brief get node id of all the nodes establishing connection with the current node
   * @return
   * return node id of all the nodes establishing connection with the current node
  */
  virtual vector<string> GetConnectedNodeIDs() = 0;
};
