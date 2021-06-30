import io_channel
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

   # 启动服务
    channel = io_channel.create_channel("p1", strJson, callback)

    # 创建网络拓扑
    print("start to recv========" )
    # 
    # 获取io接口
    io_channel.set_channel(channel)
    remote_nodeid = "p0"
    msgid = "1"
    data = "this is test p0"
    msg_len = len(data)
    while(True):
        recv_msg = io_channel.recv_msg(remote_nodeid, msgid, msg_len)
        print("recv_msg:{}".format(recv_msg))
    
