import channel_sdk.pyio as io
io_channel = io.APIManager()
import faulthandler
import traceback
import os
from config import cfg
from protos import via_svc_pb2

faulthandler.enable()

file_path = os.path.split(os.path.realpath(__file__))[0]
config_file = os.path.join(file_path, 'config/config.json')
with open(config_file, 'r') as load_f:
    strJson = load_f.read()

def fun(a, b, c, d, e):
    print("nodeid:{}, id:{}, errno:{}, error_msg:{}, ext_data:{}".format(a, b, c, d, e))
    return

if __name__ == '__main__':
    api = io_channel.impl.api()
    node_id = "p0"
    # 启动服务
    channel_ = api.create_channel(node_id, strJson, fun)

    # 获取via地址
    via_address = channel_.GetCurrentVia()
    print("current via address:{}".format(via_address))
    cfg['via_svc'] = via_address

    # 获取服务器地址
    address = channel_.GetCurrentAddress()
    arr_ = address.split(':')
    ip = arr_[0]
    port = arr_[1]
    cfg['bind_ip'] = ip
    cfg['port'] = port
    print("current server ip:{}, port:{}".format(ip, port))

    # 获取taskid
    taskid = channel_.GetTaskId()
    print("get task id:{}".format(taskid))

    # 注册服务
    if cfg['pass_via']:
        from via_svc.svc import expose_me

        expose_me(cfg, taskid, via_svc_pb2.NET_COMM_SVC, node_id)

    print('io demo ready for action.')

    data = ""
    # 获取数据
    channel_.recv("p1", "1", data, 100)
    print("recv data over==============")
    # 挂起服务
    api.wait_server()

