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


int main(int argc, char *argv[])
{
    //文件名
    string fn = "config.json";
    string io_config_str = readFileIntoString(fn);
    cout << io_config_str << endl;

    // 启动服务
    string address = "";
    bool is_start_server = false;
    string self_nid = "p1";
    IChannel* ptr_channel = 
            IoChannelImpl::Instance()->CreateIoChannel(self_nid, io_config_str, nullptr);

    cout << "create io channel succeed, self nodeid:" << self_nid << endl;
    // 创建channel
    string send_nodeid = "p0";
    string send_msg_id = "0x1111";
    string data = "this is p1 client send msg!";
    int64_t timeout = 10000;
    cout << "start send111========" << endl;
    ptr_channel->Send(send_nodeid.c_str(), send_msg_id.c_str(), data.c_str(), data.size(), timeout);

    this_thread::sleep_for(std::chrono::milliseconds(timeout));
    cout << "start send222========" << endl;
    ptr_channel->Send(send_nodeid.c_str(), send_msg_id.c_str(), data.c_str(), data.size(), timeout);

    cout << "start cycle to send========" << endl;
    timeout = 5000;
    int i = 0;
    while(true) 
    {
        this_thread::sleep_for(std::chrono::milliseconds(timeout));
        ++i;
        data = std::to_string(i);
        cout << "Send, nodeid:" << send_nodeid << ", msg_id:"  << send_msg_id 
            << ", data:" << data << endl;
        ptr_channel->Send(send_nodeid.c_str(), send_msg_id.c_str(), data.c_str(), data.size(), timeout);
    }
    
    return 0;
}