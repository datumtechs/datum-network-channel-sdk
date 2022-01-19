// file client_connection.h
#pragma once
#include <condition_variable>
#include <mutex>
#include <unordered_map>
#include <iostream>
#include <atomic>
#include <memory.h>
#include <queue>
#include "cycle_buffer.h"
using namespace std;

typedef vector<unsigned char> bytes;

/* 
	其他客户端连接过来，保存数据
*/
class ClientConnection
{
public:
	ClientConnection(const string& nodeid);
	~ClientConnection(){};

	ssize_t recv(const string& msgid, char* data, uint64_t length, int64_t timeout);
	bool write(const string& msgid, const bytes& data);
	void SetRecvTimeOut(const uint64_t recv_timeout) {
		recv_timeout_ = recv_timeout;
	}
public:
	uint64_t recv_timeout_; // Timeout period for receive data each time, Unit:millisecond
	string nodeid_;
	mutex mutex_;
	condition_variable cv_;
#if USE_BUFFER
	unordered_map<string, shared_ptr<cycle_buffer>> mapbuffer_;
#else
	unordered_map<string, shared_ptr<queue<bytes>>> mapbuffer_;
#endif
};

using MapClientConn = unordered_map<string, shared_ptr<ClientConnection>>;
