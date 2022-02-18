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
        channel = io_channel.create_channel("p0", strJson)
        remote_nodeid = "p1"
        msgid = "1"
        data = "this is test p0 client."
        msg_len = len(data)

        # print("start send to p1========" )
        while(True):
            # print("start recv from p1========" )
            io_channel.Send(remote_nodeid, msgid, data, msg_len, 0)
            recv_data = io_channel.Recv(remote_nodeid, msgid, 100, 0)
            print("recv data:{}".format(recv_data))
            time.sleep(2)

    except Exception as e:
        print(traceback.format_exc())