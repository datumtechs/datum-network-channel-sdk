// file client_connection.h
#pragma once
#include <condition_variable>
#include <mutex>
#if USE_BUFFER
#include "cycle_buffer.h"
#endif
#include <unordered_map>
#include <iostream>
#include <atomic>
#include <map>
#include <memory.h>
#include <queue>
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

public:
	string nodeid_;
#if USE_BUFFER
	shared_ptr<cycle_buffer> buffer_ = nullptr;
	//! for one message which id is msg_id
	map<string, shared_ptr<cycle_buffer>> mapbuffer_;
	std::mutex mapbuffer_mtx_;
#else
	// bool write(const string& msgid, const string& data);
	bool write(const string& msgid, const bytes& data);
	mutex mutex_;
	condition_variable cv_;
	unordered_map<string, shared_ptr<queue<bytes>>> map_queue_;
#endif

};
