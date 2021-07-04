#pragma once
#include "io_channel_impl.h"
#include <string>
#include <fstream>
#include <sstream>
#include <chrono>
#include <iostream>
#include <stdlib.h>
#include <unistd.h>
using namespace std;

/*
多对多：多个客户端发送，一个服务器接收
*/
const int server_counts = 2;
int64_t timeout = 1000 * 1000000;

//从文件读入到string里
string readFileIntoString(const string& filename)
{
    ifstream ifile(filename);
    if(!ifile.good())
    {
        cout << "file is not exist:" << filename << endl;
        return "";
    }
    //将文件读入到ostringstream对象buf中
    ostringstream buf;
    char ch;
    while(buf&&ifile.get(ch))
    buf.put(ch);
    //返回与流对象buf关联的字符串
    return buf.str();
}

class Send_Object {
public:
    Send_Object(shared_ptr<IChannel> channel, const string& self_nodeid, const string& send_data, 
        uint64_t msg_len, uint64_t record_count):channel_(channel), 
        self_nodeid_(self_nodeid), send_data_(send_data), msg_len_(msg_len), record_count_(record_count)
        {}
    
    void set_start_time(chrono::steady_clock::time_point start_time)
    {
        start_time_ = start_time;
    }

    bool send(const string sid)
    {
        string send_msg_id = "";
        for(int i = 0; i < record_count_; ++i)
        {
            send_msg_id = to_string(i);
            channel_->Send(sid.c_str(), send_msg_id.c_str(), send_data_.c_str(), msg_len_, timeout);
            // cout << "send to nodeid: " << sid << ", data+msgid: " << send_msg_id << endl;
        }

        auto end_time = std::chrono::steady_clock::now();
        std::chrono::duration<double, std::micro> elapsed = end_time - start_time_;
	    cout << "[send] self nodeid:" << self_nodeid_ << ", server nodeid:" << sid << ", 总占用时间:"
            << (double)elapsed.count() / 1000000 << " (s)" << endl;
       
        return true;
    }
private:
    shared_ptr<IChannel> channel_;
    uint64_t msg_len_;
    string send_data_;
    string self_nodeid_;
    chrono::steady_clock::time_point start_time_;
    uint64_t record_count_ = 0; // 记录数
};

class Recv_Object {
public:
    Recv_Object(shared_ptr<IChannel> channel, const string self_nodeid, uint64_t msg_len, 
            uint64_t record_count):channel_(channel),self_nodeid_(self_nodeid), msg_len_(msg_len), 
            record_count_(record_count){}
    
    void set_start_time(chrono::steady_clock::time_point start_time)
    {
        start_time_ = start_time;
    }

    bool recv(const string cid)
    {
        // cout << "start recv=====" << endl;
        string recv_msg_id = "";
        for(int i = 0; i < record_count_; ++i)
        {
            string str(msg_len_, 0);
            recv_msg_id = to_string(i);
            channel_->Recv(cid.c_str(), recv_msg_id.c_str(), &str[0], msg_len_, timeout);
            // cout << "recv by nodeid: " << cid << ", data+msgid: " << recv_msg_id << endl;
        }

        // cout << "recv over=====" << endl;
        auto end_time = std::chrono::steady_clock::now();
        std::chrono::duration<double, std::micro> elapsed = end_time - start_time_;
        cout << "[recv] self nodeid:" << self_nodeid_ << ", client nodeid:" << cid << ", 总占用时间:"
            << (double)elapsed.count() / 1000000 << " (s)" << endl;
       
        return true;
    }
private:
    shared_ptr<IChannel> channel_;
    uint64_t msg_len_;
    string self_nodeid_;
    chrono::steady_clock::time_point start_time_;
    uint64_t record_count_ = 0;
};

