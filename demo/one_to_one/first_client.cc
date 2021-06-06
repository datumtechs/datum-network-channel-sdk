#include "connection.h"
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <stdlib.h>
using namespace std;

/*
先连接客户端，后启动服务器，然后再发送数据
*/
int main(int argc, char *argv[])
{
    for(int i=0; i< argc; i++)  
        cout<<"Argument "<< i <<" is "<< argv[i] << endl; 

    cout << "start to create io channel!" << endl;
    string self_nodeid = "0x111";
    string address = "127.0.0.1:50041";
    ClientConnection cnn(self_nodeid, address);
    string msg_id = "this is msg id";
    string data = "this is send data";
    int64_t timeout = 100;

    cout << "start sleep!" << endl;
    int64_t sleep_time = 10000;
    if(argc >= 2)
    {
        sleep_time = int64_t(argv[1]);
    }
    this_thread::sleep_for(std::chrono::milliseconds(sleep_time));
    cout << "start to call send===========" << endl;
    cnn.send(msg_id, data, timeout);

    return 0;
}

