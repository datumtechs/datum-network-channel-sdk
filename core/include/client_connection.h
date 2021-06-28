// file client_connection.h
#pragma once
#include "cycle_buffer.h"
#include <unordered_map>
#include <mutex>
#include <iostream>
#include <atomic>
#include <map>
#include <memory.h>
using namespace std;

/* 
	其他客户端连接过来，保存数据
*/
class ClientConnection
{
public:
	ClientConnection();
	~ClientConnection(){}

	ssize_t recv(const string& id, char* data, uint64_t length, int64_t timeout);
public:
	shared_ptr<cycle_buffer> buffer_ = nullptr;
	//! for one message which id is msg_id
	map<string, shared_ptr<cycle_buffer>> mapbuffer_;
	std::mutex mapbuffer_mtx_;
};
