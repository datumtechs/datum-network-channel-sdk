import channel_sdk.pyio as io
io_channel = io.APIManager()
import faulthandler
import json
import os
import time

if __name__ == '__main__':
    file_name = 'one_to_one.json'
    file_name = 'p0_client_config.json'
    file_path = os.path.split(os.path.realpath(__file__))[0]
    config_file = os.path.join(file_path, file_name)
    with open(config_file, 'r') as load_f:
        strJson = load_f.read()
        # strJson = os.path.expandvars(strJson)
        print(strJson)

    try:
        # 启动服务
        nodeid = "p0"
        channel = io_channel.create_channel(nodeid, strJson)
        print("create channel succeed, nodeid:{}============".format(nodeid))
        remote_nodeid = "p1"
        data = "this is test p0 client."
        # print("start send to p1========" )
        while(True):
            # print("start recv from p1========" )
            io_channel.Send(remote_nodeid, data)
            recv_data = io_channel.Recv(remote_nodeid, 16)
            print("[client] recv bytes:{}, decode data:{}".format(recv_data, recv_data.decode()))
            time.sleep(2)

    except Exception as e:
        print("exception:", e)
        