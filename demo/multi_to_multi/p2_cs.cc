#include "io_channel_impl.h"
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <stdlib.h>
#include <unistd.h>
using namespace std;


//从文件读入到string里
string readFileIntoString(const string& filename)
{
    ifstream ifile(filename);
    //将文件读入到ostringstream对象buf中
    ostringstream buf;
    char ch;
    while(buf&&ifile.get(ch))
    buf.put(ch);
    //返回与流对象buf关联的字符串
    return buf.str();
}

const int max_time = 10000;

bool start_send(GRpcChannel& channel_)
{
    // 多线程接收
    auto send_f = [&](const string& sid) -> bool {
        // 
        string send_msg_id = "";
        for(int i = 0; i < max_time; ++i)
        {
            send_msg_id = to_string(i);
            string data = "this is send from " + sid + " client, msg_id is ======= " + send_msg_id;
            channel_.Send(sid, send_msg_id, data);
            // cout << "send by nodeid: " << sid << ", data: " << data << endl;
            usleep(1500);
        }
       
        return true;
    };

    vector<string> vec_nid(3);
    vec_nid[0] = "p0";
    vec_nid[1] = "p1";
    vec_nid[2] = "p9";
    
    vector<thread> vec_thread(3);
    for(int i = 0; i < vec_nid.size(); i++)
    {
        vec_thread[i] = thread(send_f, vec_nid[i]);
        vec_thread[i].join();
    }
    return true;
}

bool start_recv(GRpcChannel& channel_)
{
    // 多线程接收
    auto recv_f = [&](const string& cid) -> bool {
        // 
        for(int i = 0; i < max_time; ++i)
        {
            string data = "";
            cout << "start recv=========" << endl;
            usleep(3000);
            channel_.Recv(cid, to_string(i), data);
            cout << "recv by nodeid: " << cid << ", data: " << data << endl;
        }
       
        return true;
    };

    vector<string> vec_nid(3);
    vec_nid[0] = "p0";
    vec_nid[1] = "p1";
    vec_nid[2] = "p9";
    
    vector<thread> vec_thread(3);
    cout << "start recv=========" << endl;
    for(int i = 0; i < vec_nid.size(); i++)
    {
        vec_thread[i] = thread(recv_f, vec_nid[i]);
        vec_thread[i].join();
    }
    return true;
}

int main(int argc, char *argv[])
{
    //文件名
    string fn = "config.json";
    string io_config_str = readFileIntoString(fn);
    cout << io_config_str << endl;

    IoChannelImpl io_impl;

    // 启动服务
    string address = "127.0.0.1:11113";
    bool is_start_server = true;
    // 创建io
    shared_ptr<BasicIO> io_ = io_impl.CreateChannel("p2", io_config_str, 
            is_start_server, address);

    cout << "start send to p0, p1, p9========" << endl;
    GRpcChannel channel_(io_);
    cout << "start sleep!" << endl;
    int64_t sleep_time = 2000;
    string data = "";
    start_send(channel_);
    while(true)
    {
        // 读到第一条记录开始
        channel_.Recv("p0", "0", data);
        if("" != data)
        {
            break;
        }
        usleep(sleep_time*10);
    }
    start_recv(channel_);

    io_impl.CloseServer();
    if(is_start_server)
    {
        io_impl.Wait_Server();
    }
    
    return 0;
}

