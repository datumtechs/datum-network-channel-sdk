import channel_sdk.grpc as grpc
io_channel = grpc.APIManager()
import faulthandler
import json
import os
import time
import traceback

faulthandler.enable()

file_path = os.path.split(os.path.realpath(__file__))[0]
config_file = os.path.join(file_path, 'config/one_to_one.json')
with open(config_file, 'r') as load_f:
    strJson = load_f.read()

def callback(a, b, c, d, e):
    print("nodeid:{}, id:{}, errno:{}, error_msg:{}, ext_data:{}".format(a, b, c, d, e))
    return

if __name__ == '__main__':
    try:

        # 启动服务
        channel = io_channel.create_channel("p0", strJson, callback)

        # 创建网络拓扑
        print("start to send========" )
        # 
        # 获取io接口
        # io_channel.set_channel(channel)
        remote_nodeid = "p1"
        msgid = "1"
        data = "this is test p0"
        io_channel.send_msg(remote_nodeid, msgid, data, 100, 0)

    except Exception as e:
        print(traceback.format_exc())