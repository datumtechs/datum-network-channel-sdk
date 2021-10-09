#!/usr/bin/env python3
import datetime
import latticex.rosetta as rtt  # difference from tensorflow
import math
import os
import csv
import tensorflow as tf
import numpy as np
from util import read_dataset

import datetime
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
    print("_node_id======================:{}".format(node_id_))
    # 启动服务
    res = io_channel.create_channel(node_id_, strJson)
    return res


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

channel = create_channel()
# 注册服务
if cfg['pass_via']:
    from via_svc.svc import expose_me
    expose_me(cfg, task_id, via_svc_pb2.NET_COMM_SVC, node_id_)

rtt.set_channel("", channel)
print("set channel succeed==================")

np.set_printoptions(suppress=True)

os.environ['TF_CPP_MIN_LOG_LEVEL'] = '3'

np.random.seed(0)

EPOCHES = 10
BATCH_SIZE = 16
learning_rate = 0.0002

rtt.activate("SecureNN")
mpc_player_id = rtt.py_protocol_handler.get_party_id()

# real data
# ######################################## difference from tensorflow
file_x = './dsets/P' + str(mpc_player_id) + "/cls_train_x.csv"
file_y = './dsets/P' + str(mpc_player_id) + "/cls_train_y.csv"
real_X, real_Y = rtt.PrivateDataset(data_owner=(
    0, 1), label_owner=0).load_data(file_x, file_y, header=None)
# ######################################## difference from tensorflow
DIM_NUM = real_X.shape[1]
print(real_X)
print(real_X.shape)

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
pred_Y = tf.sigmoid(tf.matmul(X, W) + b)
print(pred_Y)

# loss
logits = tf.matmul(X, W) + b
loss = tf.nn.sigmoid_cross_entropy_with_logits(labels=Y, logits=logits)
loss = tf.reduce_mean(loss)
print(loss)

# optimizer
train = tf.train.GradientDescentOptimizer(learning_rate).minimize(loss)
print(train)

init = tf.global_variables_initializer()
print(init)

# ########### for test, reveal
reveal_W = rtt.SecureReveal(W)
reveal_b = rtt.SecureReveal(b)
reveal_Y = rtt.SecureReveal(pred_Y)
# ########### for test, reveal

with tf.Session() as sess:
    sess.run(init)
    rW, rb = sess.run([reveal_W, reveal_b])
    print("init weight:{} \nbias:{}".format(rW, rb))

    # train
    BATCHES = math.ceil(len(real_X) / BATCH_SIZE)
    for e in range(EPOCHES):
        for i in range(BATCHES):
            bX = real_X[(i * BATCH_SIZE): (i + 1) * BATCH_SIZE]
            bY = real_Y[(i * BATCH_SIZE): (i + 1) * BATCH_SIZE]
            sess.run(train, feed_dict={X: bX, Y: bY})

            j = e * BATCHES + i
            if j % 50 == 0 or (j == EPOCHES * BATCHES - 1 and j % 50 != 0):
                rW, rb = sess.run([reveal_W, reveal_b])
                print("I,E,B:{:0>4d},{:0>4d},{:0>4d} weight:{} \nbias:{}".format(
                    j, e, i, rW, rb))

    # predict
    Y_pred = sess.run(reveal_Y, feed_dict={X: real_X, Y: real_Y})
    print("Y_pred:", Y_pred)

print(rtt.get_perf_stats(True))
rtt.deactivate()

print("over=============")
end_time = datetime.datetime.now()
print("当前时间:{}, 总共用时:{}".format(end_time.strftime('%Y/%m%d %H:%M:%S'), (end_time - start_time)))
