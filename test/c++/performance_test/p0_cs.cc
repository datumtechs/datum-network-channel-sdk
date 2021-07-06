#include "common.h"

/*
多对多：多个客户端发送，一个服务器接收
*/
const string self_nid = "p0";
// const string send_data = "hello world, I am p0->";
// const uint64_t msg_len = send_data.size();
string array_nid[] = {"p1", "p2"};
vector<string> vec_nid(array_nid, array_nid+server_counts);

int main(int argc, char *argv[])
{
    if(argc < 3)
    {
        cout << "[command] [bytes] [record_count]" << endl;
        return 0;
    }

    string data_file = string(argv[1]) + "_bytes.txt";
    uint64_t record_count = stoi(argv[2]);

    string send_data = readFileIntoString(data_file);
    if("" == send_data) return 0;
    uint64_t msg_len = send_data.size();

    auto start_time = std::chrono::steady_clock::now();

    //文件名
    string fn = "config.json";
    string io_config_str = readFileIntoString(fn);
    // cout << io_config_str << endl;

    // 启动服务
    shared_ptr<IChannel> channel = CreateChannel(self_nid, io_config_str, nullptr);

    Send_Object send_object(channel, self_nid, send_data, msg_len, record_count);
    Recv_Object recv_object(channel, self_nid, msg_len, record_count);
    vector<thread> vec_send_thread(server_counts);
    vector<thread> vec_recv_thread(server_counts);
    cout << "start send/recv=========" << endl;
    send_object.set_start_time(start_time);
    recv_object.set_start_time(start_time);
    for(int i = 0; i < vec_nid.size(); i++)
    {
        vec_send_thread[i] = thread(&Send_Object::send, &send_object, vec_nid[i]);
        vec_recv_thread[i] = thread(&Recv_Object::recv, &recv_object, vec_nid[i]);
    //    vec_thread[i].join();
    }
    while(true);
    
    return 0;
}

