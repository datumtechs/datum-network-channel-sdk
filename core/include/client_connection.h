// file client_connection.h
#pragma once
#include <condition_variable>
#include <mutex>
#include <unordered_map>
#include <iostream>
#include <atomic>
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
	bool write(const string& msgid, const bytes& data);
	mutex mutex_;
	condition_variable cv_;
	unordered_map<string, shared_ptr<queue<bytes>>> map_queue_;
};

using MapClientConn = unordered_map<string, shared_ptr<ClientConnection>>;
