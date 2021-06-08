import lib.io_channel as io_channel
import faulthandler
import json
import os
import time
import traceback

faulthandler.enable()

file_path = os.path.split(os.path.realpath(__file__))[0]
config_file = os.path.join(file_path, 'config/config.json')
with open(config_file, 'r') as load_f:
    strJson = load_f.read()

def fun(a, b, c, d, e):
    print("nodeid:{}, id:{}, errno:{}, error_msg:{}, ext_data:{}".format(a, b, c, d, e))
    return

if __name__ == '__main__':
    try:

        # 启动服务
        address = ""
        is_start_server = False

        api = io_channel.impl.api()

        io_ = api.create_channel("p1", strJson, is_start_server, address, fun)

        # time.sleep(1000)
        # print(io_)

        # 创建网络拓扑
        print("start to send========" )
        # 
        # 获取io接口
        channel_ = io_channel.impl.grpc(io_)
        channel_.send("p0", "1", "this is test p1", 1)

        if(is_start_server):
            api.wait_server()
            
    except Exception as e:
        print(traceback.format_exc())