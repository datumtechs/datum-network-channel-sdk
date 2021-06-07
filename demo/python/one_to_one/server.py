import lib.io_channel as io_channel
import faulthandler
import traceback

faulthandler.enable()

if __name__ == '__main__':

    api = io_channel.impl.api()
    address = "127.0.0.1:11111"
    # 启动服务
    api.start_server(address)
    # 挂起服务
    api.wait_server()

