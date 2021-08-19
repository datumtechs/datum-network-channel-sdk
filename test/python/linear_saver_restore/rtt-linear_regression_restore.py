#!/usr/bin/env python3
import latticex.rosetta as rtt  # difference from tensorflow
import math
import os
import csv
import tensorflow as tf
import numpy as np
from util import read_dataset


import datetime
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


# 使用外部io
using_external_io = True
if using_external_io:
    print("use external io=================================")
    import channel_sdk.grpc as grpc
    io_channel = grpc.APIManager()
    channel = io_channel.create_channel(node_id_, strJson)
    cfg = {'pass_via': False}
    # 注册服务
    if cfg['pass_via']:
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
        from via_svc.svc import expose_me
        # expose_me(cfg, task_id, via_svc_pb2.NET_COMM_SVC, node_id_)

    # 使用外部io
    rtt.set_channel("", channel)
    print("set channel succeed==================")

np.set_printoptions(suppress=True)

os.environ['TF_CPP_MIN_LOG_LEVEL'] = '3'

np.random.seed(0)

EPOCHES = 10
BATCH_SIZE = 16
learning_rate = 0.0002

rtt.py_protocol_handler.set_loglevel(0)
rtt.activate("Helix")
rtt.py_protocol_handler.set_loglevel(7)
rtt.py_protocol_handler.set_restore_model(['p30'])
#rtt.py_protocol_handler.set_restore_model(['p0', 'p1', 'p2'])
node_id = rtt.get_io_wrapper().get_current_node_id()

# real data
# ######################################## difference from tensorflow
file_x = '../dsets/' + node_id + "/reg_test_x.csv"
file_y = '../dsets/' + node_id + "/reg_test_y.csv"
real_X, real_Y = rtt.PrivateDataset(data_owner=(
    'p0', 'p1'), label_owner='p1').load_data(file_x, file_y, header=None)
# ######################################## difference from tensorflow
DIM_NUM = real_X.shape[1]

X = tf.placeholder(tf.float64, [None, DIM_NUM])
Y = tf.placeholder(tf.float64, [None, 1])
print(X)
print(Y)

# initialize W & b
W = tf.Variable(tf.zeros([DIM_NUM, 1], dtype=tf.float64))
b = tf.Variable(tf.zeros([1], dtype=tf.float64))
print(W)
print(b)

# predict
pred_Y = tf.matmul(X, W) + b
print(pred_Y)


# save
saver = tf.train.Saver(var_list=None, max_to_keep=5, name='v2')
os.makedirs("./log/ckpt_" + node_id, exist_ok=True)

# init
init = tf.global_variables_initializer()
reveal_Y = rtt.SecureReveal(pred_Y)
with tf.Session() as sess:
    sess.run(init)
    if os.path.exists('./log/ckpt_' + node_id +'/checkpoint'):
        saver.restore(sess, './log/ckpt_' + node_id + '/model')
    print('X:', X)
    print('W:', W)

    # predict
    Y_pred = sess.run(pred_Y, feed_dict={X: real_X})
    print("Y_pred:", Y_pred)

    reveal_y = sess.run(reveal_Y, feed_dict={X: real_X})
    print("reveal_Y:", reveal_y)

print(rtt.get_perf_stats(True))
rtt.deactivate()

print("over=============")
end_time = datetime.datetime.now()
print("当前时间:{}, 总共用时:{}".format(end_time.strftime('%Y/%m%d %H:%M:%S'), (end_time - start_time)))
