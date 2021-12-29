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
    string address = "127.0.0.1:11111";
    bool is_start_server = true;
    string self_nid = "p0";

    IChannel* ptr_channel = 
            IoChannelImpl::Instance()->CreateIoChannel(self_nid, io_config_str, nullptr);

    cout << "create io channel succeed, self nodeid:" << self_nid << endl;
    cout << "start sleep!" << endl;
    int64_t sleep_time = 100;
    if(argc >= 2)
    {
        sleep_time = int64_t(argv[1]);
    }
    // this_thread::sleep_for(std::chrono::milliseconds(sleep_time));

    cout << "start to call recv1========" << endl;
    string send_nodeid = "p1";
    string send_msg_id = "0x1111";
    // string data = "";
    char szData[1024] = "";
    ptr_channel->Recv(send_nodeid.c_str(), send_msg_id.c_str(), szData, 100);
    cout << "recv from:" << send_nodeid << ", szData: " << szData << endl;

    while(true)
    {
        // cout << "start sleep!" << endl;
        // this_thread::sleep_for(std::chrono::milliseconds(sleep_time));
        // cout << "start to call recv2========" << endl;
        // data = "";
        memset(szData, 0, 1024);
        ptr_channel->Recv(send_nodeid.c_str(), send_msg_id.c_str(), szData, 100);
        cout << "recv from:" << send_nodeid << ", szData: " << szData << endl;
    }
    
    return 0;
}