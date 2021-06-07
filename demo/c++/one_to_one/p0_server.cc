#include "io_channel_impl.h"
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <stdlib.h>
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


int main(int argc, char *argv[])
{
    //文件名
    string fn = "config.json";
    string io_config_str = readFileIntoString(fn);
    cout << io_config_str << endl;

    IoChannelImpl io_impl;

    // 启动服务
    string address = "127.0.0.1:11111";
    bool is_start_server = true;
    shared_ptr<BasicIO> io_ = io_impl.CreateChannel("p0", io_config_str, 
            is_start_server, address, nullptr);

    cout << "start sleep!" << endl;
    int64_t sleep_time = 10000;
    if(argc >= 2)
    {
        sleep_time = int64_t(argv[1]);
    }
    this_thread::sleep_for(std::chrono::milliseconds(sleep_time));

    cout << "start to call recv1========" << endl;
    GRpcChannel channel_(io_);
    string send_nodeid = "p1";
    string send_msg_id = "0x1111";
    string data = "";
    channel_.Recv(send_nodeid, send_msg_id, data);
    cout << "recv from:" << send_nodeid << ", data: " << data << endl;

    // while(true)
    {
        cout << "start sleep!" << endl;
        this_thread::sleep_for(std::chrono::milliseconds(sleep_time));
        cout << "start to call recv2========" << endl;
        data = "";
        channel_.Recv(send_nodeid, send_msg_id, data);
        cout << "recv from:" << send_nodeid << ", data: " << data << endl;
    }
    io_impl.CloseServer();
    if(is_start_server)
    {
        io_impl.WaitServer();
    }
    
    return 0;
}

