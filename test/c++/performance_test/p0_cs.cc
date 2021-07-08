#include "common.h"

/*
多对多：多个客户端发送，一个服务器接收
*/
const string self_nid = "p0";
// const string send_data = "hello world, I am p0->";
// const uint64_t msg_len = send_data.size();
string array_nid[] = {"p1", "p2"};
vector<string> other_nids(array_nid, array_nid+server_counts);

int main(int argc, char *argv[])
{
    if(argc < 3)
    {
        cout << "[command] [bytes] [record_count]" << endl;
        return 0;
    }

    string data_file = string(argv[1]) + "_bytes.txt";
    uint64_t record_count = stoi(argv[2]);

    RunTest test;

    if(!test.start_test(data_file, record_count, self_nid, other_nids))
    {
        return 0;
    }
    while(true);
    
    return 0;
}

