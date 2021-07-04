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

### 创建编译目录

```bash
cd channel-sdk && mkdir build && cd build
```

### 编译选项说明

注意：编译选项可组合使用；

- 指定使用目标

  io库可以独立使用，也可以给rosetta使用；

  ```
  cmake -DUSE_ALONE=1
  ```

  > 说明：
  >
  > - USE_ALONE=1表示独立使用；
  > - 给rosetta使用时（默认），不需要指定；



- 指定服务器类型

  包括异步和同步两种类型；

  ```bash
  cmake -DASYNC_SERVER=1
  ```

  > 说明：
  >
  > - ASYNC_SERVER=1表示使用异步服务器；
  > - 不指定时（默认）表示使用同步服务器；



- 指定客户端类型

  包括异步和同步两种类型；

  ```bash
  cmake -DASYNC_CLIENT=1
  ```

  > 说明：
  >
  > - ASYNC_CLIENT=1表示使用异步客户端（不需要等待服务器的响应，直接返回）；
  > - 不指定时（默认）表示使用同步客户端；



- 指定python版本

  ```bash
  cmake -DPYBIND11_PYTHON_VERSION=3.6
  ```

  > 说明：
  >
  > - PYBIND11_PYTHON_VERSION=3.6表示使用python3.6版本的解释器
  > - 不指定时使用系统安装的最高版本；

### 执行编译

```bash
make
```

> 编译成功后在build目录下生成so文件，包括io库`libexternal_io.so`和cpython库，如cpython库为：`io_channel.cpython-37m-x86_64-linux-gnu.so`，表示对应的python版本号为python3.7；



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

  

