# channel-sdk

RosettaNet网络层sdk

## 环境预安装

```bash
sudo apt-get install build-essential autoconf libtool libgflags-dev libgtest-dev clang libc++-dev pkg-config unzip
git clone -b develop http://192.168.9.66/RosettaFlow/channel-sdk.git --recursive
cd channel-sdk/third_party/protobuf
sudo ./autogen.sh
sudo ./configure
make
sudo make install
```

## 编译

```bash
cd channel-sdk && mkdir build && cd build
cmake ..
make
```

> 编译成功后在channel-sdk/lib下生成so文件，如：`io_channel.cpython-37m-x86_64-linux-gnu.so`，表示对应的python版本号为python3.7；



## 测试

可将`channel-sdk/test/python`下的例子拷贝到channel-sdk目录下，或者将lib目录拷贝到python实例目录下，

进行测试：

- 启动服务器

  ```bash
  python3 server.py
  ```

- 启动客户端

  ```bash
  python3 client.py
  ```

  

