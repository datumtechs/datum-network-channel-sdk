// file sync_client.h
#pragma once
#include "base_client.h"
#include <iostream>

using namespace std;

/*
	同步客户端
*/
class SyncClient : public BaseClient
{
public:
	SyncClient(const ViaInfo& via_info, const string& taskid);
	// // Create a connection between the node where the server is located and VIA to register the interface.
	// SyncClient(const NodeInfo& node_info, const string& taskid);
	~SyncClient(){}

  	ssize_t send(const string& self_nodeid, const string& remote_nodeid, const string& id,
	  	 const char* data, const size_t nLen, int64_t timeout = -1L);
  	// ssize_t recv(const string& id, string& data, int64_t timeout = -1L);
};
