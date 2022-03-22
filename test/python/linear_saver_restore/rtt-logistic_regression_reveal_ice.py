#!/usr/bin/env python3
import datetime
import latticex.rosetta as rtt  # difference from tensorflow
import math
import os
import csv
import tensorflow as tf
import numpy as np

import datetime
import os
import json
import argparse

_parser = argparse.ArgumentParser(description="LatticeX Rosetta")
_parser.add_argument('--node_id', type=str, help="Node ID",
                     required=False, default='')
_args, _unparsed = _parser.parse_known_args()
node_id_ = _args.node_id

configFilePath = 'config_ice.json'

file_path = os.path.split(os.path.realpath(__file__))[0]
config_file = os.path.join(file_path, configFilePath)
with open(config_file, 'r') as load_f:
    strJson = load_f.read()

# 使用外部io
using_external_io = True
io_channel = None
if using_external_io:
    print("use external io=================================")
    try:
        import channel_sdk.pyio as io
        io_channel = io.APIManager()
        channel = io_channel.create_channel(node_id_, strJson)
    except Exception as e:
        print("catch event exception:", e)
        exit(0)

    # 使用外部io
    print("start set==================")
    rtt.set_channel("", channel)
    print("set channel succeed==================")

np.set_printoptions(suppress=True)

os.environ['TF_CPP_MIN_LOG_LEVEL'] = '0'

np.random.seed(0)

EPOCHES = 10
BATCH_SIZE = 16
learning_rate = 0.0002

rtt.py_protocol_handler.set_loglevel(5)
rtt.activate("SecureNN")
start_time = datetime.datetime.now()
mpc_player_id = rtt.get_party_id()
node_id = rtt.get_current_node_id()
print("mpc_player_id:", mpc_player_id)
# rtt.set_saver_model(["p1"])
print("finish set save model.")

# real data
# ######################################## difference from tensorflow
file_x = './data/dsets_big/' + node_id + "/cls_train_x.csv"
file_y = './data/dsets_big/' + node_id + "/cls_train_y.csv"
real_X, real_Y = rtt.PrivateDataset(data_owner=(
    "p0", "p1"), label_owner="p1").load_data(file_x, file_y, header=0)

# file_x = './dsets/' + node_id + "/reg_train_x_tmp.csv"
# file_y = './dsets/' + node_id + "/reg_train_y_tmp.csv"
# real_X, real_Y = rtt.PrivateDataset(data_owner=(
#     'p0', 'p1'), label_owner='p1').load_data(file_x, file_y, header=None)
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
