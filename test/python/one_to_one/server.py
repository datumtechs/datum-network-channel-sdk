import channel_sdk.pyio as io
io_channel = io.APIManager()
import faulthandler
import json
import os
import time
import traceback


faulthandler.enable()

file_path = os.path.split(os.path.realpath(__file__))[0]
config_file = os.path.join(file_path, 'one_to_one.json')
with open(config_file, 'r') as load_f:
    strJson = load_f.read()

if __name__ == '__main__':

    try:
    # 启动服务
        channel = io_channel.create_channel("p1", strJson)

        # 创建网络拓扑
        # 
        remote_nodeid = "p0"
        data = "this is test p1 server."
        while(True):
            # print("start recv from p0========" )
            recv_data = io_channel.Recv(remote_nodeid, 100)
            print("recv data:{}".format(recv_data))

            # print("start send to p0========" )
            io_channel.Send(remote_nodeid, data)
    except Exception as e:
        print("exception:", e)
        print(traceback.format_exc())