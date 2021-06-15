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

        # 启动客户端
        is_start_server = False

        api = io_channel.impl.api()
        # 创建网络拓扑
        channel_ = api.create_channel("p1", strJson, is_start_server, fun)

        # time.sleep(1000)
        # print(io_)

    
        print("start to send========" )
        # 
        # 发送数据
        channel_.send("p0", "2", "this is test p1", 100)


        # 获取via地址
        via_address = channel_.GetCurrentVia()
        print("current via address:{}".format(via_address))
        
        # 测试io接口
        # GetDataNodeIDs
        '''
        list_data_nids = channel_.GetDataNodeIDs()
        print("get data nodes id:{}".format(list_data_nids))

        # GetComputationNodeIDs
        dict_cmpt_nids = channel_.GetComputationNodeIDs()
        print("get computation node id:{}".format(dict_cmpt_nids))

        # GetResultNodeIDs
        list_res_nids = channel_.GetResultNodeIDs()
        print("get result nodes id:{}".format(list_res_nids))

        # GetCurrentNodeID
        curr_node_id = channel_.GetCurrentNodeID()
        print("get current node id:{}".format(curr_node_id))

        # GetConnectedNodeIDs
        list_conn_nids = channel_.GetConnectedNodeIDs()
        print("get connected nodes id:{}".format(list_conn_nids))
        '''
        if(is_start_server):
            api.wait_server()
            
    except Exception as e:
        print(traceback.format_exc())