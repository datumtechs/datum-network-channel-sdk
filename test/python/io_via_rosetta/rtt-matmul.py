#!/usr/bin/env python3

# Import rosetta package
import datetime
import latticex.rosetta as rtt
import tensorflow as tf

import channel_sdk.pyio as io
io_channel = io.APIManager()
from config import cfg
from protos import via_svc_pb2
import os
import json
import argparse

start_time = datetime.datetime.now()

_parser = argparse.ArgumentParser(description="LatticeX Rosetta")
_parser.add_argument('--node_id', type=str, help="Node ID",
                     required=False, default='')
_args, _unparsed = _parser.parse_known_args()
node_id_ = _args.node_id

file_path = os.path.split(os.path.realpath(__file__))[0]
config_file = os.path.join(file_path, 'config/config.json')
with open(config_file, 'r') as load_f:
    strJson = load_f.read()

file_path = os.path.split(os.path.realpath(__file__))[0]
config_file = os.path.join(file_path, 'config/config.json')
with open(config_file, 'r') as load_f:
    strJson = load_f.read()

config_dict = json.loads(strJson)


def get_current_via_address():
    list_node_info = config_dict["NODE_INFO"]
    address = ""
    via = ""
    for node_info in list_node_info:
        nodeid = node_info["NODE_ID"]
        if nodeid == node_id_:
            address = node_info["ADDRESS"]
            via = node_info["VIA"]
            break

    if "" == via:
        return "", ""

    via_info = config_dict["VIA_INFO"]
    via_address = via_info[via]

    return address, via_address


def get_task_id():
    return config_dict["TASK_ID"]


def fun(a, b, c, d, e):
    print("nodeid:{}, id:{}, errno:{}, error_msg:{}, ext_data:{}".format(a, b, c, d, e))
    return


def create_channel():
    res = io_channel.create_channel(node_id_, strJson)
    return res


# def start_via():
print("_node_id======================:{}".format(node_id_))
# 启动服务
channel = create_channel()

# 注册到via========================================
# 获取via地址
address, via_address = get_current_via_address()
print("========current address:{}, current via address:{}".format(address, via_address))
task_id = get_task_id()
print("========task id:{}".format(task_id))

arr_ = address.split(':')
ip = arr_[0]
port = arr_[1]
cfg['via_svc'] = via_address
cfg['bind_ip'] = ip
cfg['port'] = port
print("current server ip:{}, port:{}".format(ip, port))


# 注册服务
def start_via():
    if cfg['pass_via']:
        from via_svc.svc import expose_me
        expose_me(cfg, task_id, via_svc_pb2.NET_COMM_SVC, node_id_)

    rtt.set_channel("", channel)
    print("set channel succeed==================")


# def channel_test():
# You can activate a backend protocol, here use SecureNN
rtt.set_backend_loglevel(0)


def channel_test():
    rtt.activate("SecureNN")
    rtt.py_protocol_handler.set_loglevel(0)

    channel = rtt.py_io_handler.get_io_wrapper()
    print('node party', channel.get_current_node_id(), channel.get_party_id(channel.get_current_node_id()))
    print('party node', channel.get_current_party_id(), channel.get_node_id(channel.get_current_party_id()))
    print('data nodes', channel.get_data_node_ids())
    print('computation nodes', channel.get_computation_node_ids())
    print('result nodes', channel.get_result_node_ids())
    nodes = channel.get_connected_node_ids()
    node = channel.get_current_node_id()
    msgid = 'test'
    for n in nodes:
        if n != node:
            channel.send_msg(n, msgid, node + " to:" + n)
    for n in nodes:
        if n != node:
            msg = channel.recv_msg(n, msgid, 2 * len(node) + 4)
            print('get msg from ', n, " msg:", msg)


def run_rosetta():
    # You can activate a backend protocol, here use SecureNN
    rtt.activate("SecureNN")
    rtt.py_protocol_handler.set_loglevel(0)

    # Get private data from Alice (input x), Bob (input y)
    x = tf.Variable(rtt.private_input(0, [[1, 2], [2, 3]]))
    y = tf.Variable(rtt.private_input(1, [[1, 2], [2, 3]]))

    # Define matmul operation
    res = tf.matmul(x, y)

    # Start execution
    with tf.Session() as sess:
        sess.run(tf.global_variables_initializer())
        res = sess.run(res)

        # Get the result of Rosetta matmul
        # ret: [[b'14.000000' b'20.000000'] [b'20.000000' b'29.000000']]
        print('matmul:', sess.run(rtt.SecureReveal(res)))


start_via()

# channel_test()

run_rosetta()


rtt.deactivate()

print("over=============")
end_time = datetime.datetime.now()
print("当前时间:{}, 总共用时:{}".format(end_time.strftime('%Y/%m%d %H:%M:%S'), (end_time - start_time)))
