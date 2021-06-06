#include "io_channel_impl.cc"


int main(int argc, char *argv[])
{
    string address = "127.0.0.1:50041";
    IoChannelImpl server;
    cout << "start to server!" << endl;
    server.StartServer(address);

    // cout << "start to get recv!" << endl;

    server.Wait_Server();
    
    return 0;
}
