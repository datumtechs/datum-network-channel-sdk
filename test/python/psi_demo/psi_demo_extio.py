#!/usr/bin/env python3
import argparse
import time
import latticex.psi as psi
import concurrent.futures
import threading
from multiprocessing import Process
import sys
# ################################################## Args
parser = argparse.ArgumentParser(description="PSI DEMO")
parser.add_argument('--task_id', type=str, help="The task id",
                    default="task_id_test", required=False)
parser.add_argument('--node_id', type=str,
                    help="NodeId. According to `ioconfig` json file",  required=True)
parser.add_argument('--ioconfig', type=str, help="IO Config Json File",
                    default="ioconfig.json", required=False)
parser.add_argument('--psi_type', type=str, help="PSI TYPE(T_V1_Basic_SECP,T_V1_Basic_SECP_OPT,T_V2_Basic_SECP,T_V2_Basic_SECP_OPT,T_V2_Labeled_SECP,T_V2_Labeled_SECP_OPT,T_V1_Basic_GLS254,T_V1_Basic_GLS254_OPT,T_V2_Basic_GLS254,T_V2_Basic_GLS254_OPT,T_V2_Labeled_GLS254,T_V2_Labeled_GLS254_OPT)", default="T_V1_Basic_SECP", required=True)

parser.add_argument('--input', type=str,
                    help="[Y]input ids (or/and labels) file path", required=True)
parser.add_argument('--output', type=str, help="[N]result output file path",
                    default="./result/<partyid>_result.csv", required=False)

parser.add_argument('--log_prefix', type=str, help="the log prefix path for cpp",
                    default="psi", required=False)

parser.add_argument('--run_times', type=int,
                    help="how many run phases will be executed", default=1, required=False)
parser.add_argument('--use_gmssl', type=int,
                    help="0(false);1(true)|Not supported", default=0, required=False)
parser.add_argument('--threadnum', type=int,
                    help="multi-threadings supported", default=1, required=False)

parser.add_argument('--sender_bs', type=int, help="sender batch size",
                    default=1000 * 1000 * 2, required=False)
parser.add_argument('--receiver_bs', type=int, help="receiver batch size",
                    default=1000 * 1000 * 2, required=False)
parser.add_argument('--finder_bs', type=int, help="finder batch size",
                    default=1000 * 1000 * 6, required=False)
paras, unparsed = parser.parse_known_args()
# ##################################################


def getfilecontent(filepath):
    content = ""
    with open(filepath, "r", encoding="utf-8") as f:
        content = f.read()
    return content


# ##################################################


def basic_usage_mt(paras):
    cfg_json = getfilecontent(paras.ioconfig)
    print('cfg_json:', cfg_json)

    N = paras.threadnum
    threads = [None]*N

    class run_psi(threading.Thread):
        def __init__(self, tid):
            threading.Thread.__init__(self)
            self.tid = tid
            self.task_id = "taskid-" + str(tid)
            print("self.task_id:", self.task_id)

        def run(self):
            # Read some data from priavte place [write here only for convenience]
            _ = ""
            inputs = paras.input
            outputs = paras.output
            if outputs == "./result/<partyid>_result.csv":
                outputs = "./result/" + paras.node_id + "_result.csv"
            outputs += "." + self.task_id + ".csv"
            print('outputs:', outputs, flush=True)

            # Set logger
            psihandler = psi.PSIHandler()
            print('psihandler:', psihandler)
            psihandler.log_to_stdout(False)
            psihandler.set_loglevel(psi.LogLevel.Info)
            psihandler.set_pattern("%Y-%m-%d %H:%M:%S.%e|%^%l%$|%s:%#|%!|%v")
            psihandler.set_logname("log/" + paras.log_prefix + "-" +
                                   self.task_id + ".log", self.task_id)

            # IO
            iohandler = psi.IOHandler()
            io = iohandler.create_io(self.task_id, paras.node_id, cfg_json)
            # # ATTENTION! 
            # # If you use `set_channel(taskid,channel)`,
            # # you should call `get_io_wrapper(taskid)` to get the io,
            # # other more, you shouldn't call `release_io(taskid)`.

            # Init PSI Handler
            print('io:', io)
            psihandler.set_io(io, self.task_id)

            # Do PSI
            psihandler.activate(paras.psi_type, self.task_id)

            psihandler.set_sender_batchsize(paras.sender_bs, self.task_id)
            psihandler.set_receiver_batchsize(paras.receiver_bs, self.task_id)
            psihandler.set_receiver_finder_batchsize(
                paras.finder_bs, self.task_id)

            psihandler.prepare(inputs, taskid=self.task_id)
            psihandler.run(inputs, outputs, taskid=self.task_id)
            print(psihandler.get_perf_stats(True, self.task_id))
            psihandler.deactivate(self.task_id)

            # Release IO
            iohandler.release_io(self.task_id)

    for i in range(N):
        threads[i] = run_psi(i)
        threads[i].start()
    for i in range(N):
        threads[i].join()


def basic_usage_mp(paras):
    cfg_json = getfilecontent(paras.ioconfig)
    print('cfg_json:', cfg_json)

    N = paras.threadnum
    processs = [None]*N

    def run_psi(tid):
        self_task_id = "taskid-" + str(tid)
        print("self_task_id:", self_task_id)

        # Read some data from priavte place [write here only for convenience]
        _ = ""
        inputs = paras.input
        outputs = paras.output
        if outputs == "./result/<partyid>_result.csv":
            outputs = "./result/" + paras.node_id + "_result.csv"
        outputs += "." + self_task_id + ".csv"
        print('outputs:', outputs, flush=True)

        # Set logger
        psihandler = psi.PSIHandler()
        print('psihandler:', psihandler, flush=True)
        psihandler.log_to_stdout(False)
        psihandler.set_loglevel(psi.LogLevel.Info)
        psihandler.set_pattern("%Y-%m-%d %H:%M:%S.%e|%^%l%$|%s:%#|%!|%v")
        psihandler.set_logname("log/" + paras.log_prefix + "-" +
                               self_task_id + ".log", self_task_id)

        # IO
        iohandler = psi.IOHandler()
        io = iohandler.create_io(self_task_id, paras.node_id, cfg_json)
        # # ATTENTION! 
        # # If you use `set_channel(taskid,channel)`,
        # # you should call `get_io_wrapper(taskid)` to get the io,
        # # other more, you shouldn't call `release_io(taskid)`.

        # Init PSI Handler
        print('io:', io, flush=True)
        psihandler.set_io(io, self_task_id)

        # Do PSI
        psihandler.activate(paras.psi_type, self_task_id)

        psihandler.set_sender_batchsize(paras.sender_bs, self_task_id)
        psihandler.set_receiver_batchsize(paras.receiver_bs, self_task_id)
        psihandler.set_receiver_finder_batchsize(
            paras.finder_bs, self_task_id)

        psihandler.prepare(inputs, taskid=self_task_id)
        psihandler.run(inputs, outputs, taskid=self_task_id)
        print(psihandler.get_perf_stats(True, self_task_id))
        psihandler.deactivate(self_task_id)

        # Release IO
        iohandler.release_io(self_task_id)

    for i in range(N):
        processs[i] = Process(target=run_psi, args=(i,))
        processs[i].start()
    for i in range(N):
        processs[i].join()


def basic_usage_future(paras):
    print('ioconfig==========:', paras.ioconfig)
    cfg_json = getfilecontent(paras.ioconfig)
    print('cfg_json==========:', cfg_json)

    N = paras.threadnum
    all_task = []

    def run_psi(tid):
        self_task_id = "taskid-" + str(tid)
        print("self_task_id:", self_task_id)

        # Read some data from priavte place [write here only for convenience]
        _ = ""
        inputs = paras.input
        outputs = paras.output
        if outputs == "./result/<partyid>_result.csv":
            outputs = "./result/" + paras.node_id + "_result.csv"
        outputs += "." + self_task_id + ".csv"
        print('outputs:', outputs, flush=True)

        # Set logger
        psihandler = psi.PSIHandler()
        print('psihandler:', psihandler, flush=True)
        psihandler.log_to_stdout(False)
        psihandler.set_loglevel(psi.LogLevel.Info)
        psihandler.set_pattern("%Y-%m-%d %H:%M:%S.%e|%^%l%$|%s:%#|%!|%v")
        psihandler.set_logname("log/" + paras.log_prefix + "-" +
                               self_task_id + ".log", self_task_id)

        # # IO
        iohandler = psi.IOHandler()
        print("paras.node_id:", paras.node_id, flush=True)
        import channel_sdk.pyio as io
        io_channel = io.APIManager()
        channel = io_channel.create_channel(paras.node_id, cfg_json)
        iohandler.set_channel(self_task_id, channel)
        print("set_channel succeed:", paras.node_id, flush=True)

        # Do PSI
        psihandler.activate(paras.psi_type, self_task_id)

        psihandler.set_sender_batchsize(paras.sender_bs, self_task_id)
        psihandler.set_receiver_batchsize(paras.receiver_bs, self_task_id)
        psihandler.set_receiver_finder_batchsize(
            paras.finder_bs, self_task_id)

        psihandler.prepare(inputs, taskid=self_task_id)
        psihandler.run(inputs, outputs, taskid=self_task_id)
        print(psihandler.get_perf_stats(True, self_task_id))
        psihandler.deactivate(self_task_id)

        # # Release IO
        # iohandler.release_io(self_task_id)

        return True

    with concurrent.futures.ThreadPoolExecutor() as executor:
        for i in range(N):
            all_task.append(executor.submit(run_psi, i))

    concurrent.futures.wait(
        all_task, return_when=concurrent.futures.ALL_COMPLETED)
    print('all_task:', all_task)
    for one_task in all_task:
        print(one_task.result())


if __name__ == "__main__":
    # basic_usage_mt(paras)
    # basic_usage_mp(paras)
    print("start================")
    basic_usage_future(paras)
