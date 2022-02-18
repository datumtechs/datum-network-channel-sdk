# channel-sdk

RosettaNet网络层sdk

## 源码下载

```bash
git clone -b develop http://192.168.9.66/RosettaFlow/channel-sdk.git
```

## 源码编译

说明：如非源码编译方式，可不执行下面的操作步骤；

### 环境预安装

```bash
sudo apt-get install build-essential autoconf libtool libgflags-dev libgtest-dev clang libc++-dev pkg-config unzip
cd channel-sdk && git submodule update --init --recursive
cd third_party/protobuf
sudo ./autogen.sh
sudo ./configure
make
sudo make install
```

### 编译

```bash
cd channel-sdk && ./build.sh compile
```

>**编译选项说明：**
>
>- --build-type: 编译版本，包括Release（默认）和Debug；
>- --server-type：服务器类型，包括ASYNC（默认）和SYNC，ASYNC表示异步服务器，SYNC表示同步服务器；
>- --client-type：客户端类型，包括ASYNC和SYNC（默认） ，ASYNC表示异步客户端，SYNC表示同步客户端；
>- --use-buffer：是否使用循环缓存存储数据，如果是ON表示是，OFF表示否（默认为否）；
>- --static-call：是否使用静态调用方式，如果是ON表示是，OFF表示否（默认为否，即表示动态调用）；
>- --verbose：是否显示编译详情，默认否 ；
>- --ssl-type：通讯通道使用证书类型；0：不使用证书（默认），1：使用openssl库，2：使用gmssl库；
>- --python-version: 指定python版本打包; 默认为当前系统python3对应的版本, ubuntu18.04默认为：python3.6;
>
>**例子：**
>
>```bash
>./build.sh compile --build-type=Debug --static-call --use-buffer --server-type=ASYNC --verbose=1 --ssl-type=2 --python-version=3.7
>```
>
>编译成功后在dist目录下，生成`whl`安装文件；



## 安装

```bash
cd channel-sdk && ./build.sh install
```

> 将dist目录下的whl安装文件安装到python的安装目录下；安装文件主要包括：
>
> - channel sdk动态库；
> - ice相关的依赖库；
> - ice的二进制icegridregistry，glacier2router和相关的配置文件；
> - 执行和退出ice二进制服务的脚本文件；



## 删除卸载

```bash
cd channel-sdk && ./build.sh clean
```



## ICE服务

Channel Sdk通过使用IceGrid进行服务注册，和Ice的Glacier2进行消息转发，首先进入到whl的安装目录下bin目录，操作步骤如下：

### 修改配置文件

- 修改IceGrid配置文件config.gridregistry：

  ```toml
  IceGrid.InstanceName=ChannelIceGrid
  
  # IceGrid地址和ip
  Ice.Default.Locator=ChannelIceGrid/Locator:tcp -p 10032 -h 192.168.2.128
  
  IceGrid.Registry.ReplicaName=Master
  IceGrid.Registry.Client.Endpoints=tcp -p 10032 -h 192.168.2.128
  IceGrid.Registry.Server.Endpoints=tcp -h 192.168.2.128
  IceGrid.Registry.Internal.Endpoints=tcp -h 192.168.2.128
  
  IceGrid.Registry.PermissionsVerifier=ChannelIceGrid/NullPermissionsVerifier
  IceGrid.Registry.LMDB.MapSize=10
  IceGrid.Registry.LMDB.Path=deploy/lmdb/registry
  IceGrid.Registry.DynamicRegistration=1
  
  Ice.MessageSizeMax=0
  ```

  > 说明：
  >
  > 此文件主要是配置IceGrid服务的应用名称，IP地址和监听的端口；注意：配置项需要和config.json匹配；

  

- 修改Glacier2配置文件config.glacier2

  ```toml
  Glacier2.InstanceName=ChannelGlacier2
  
  # 配置IceGrid服务的ip和端口
  Ice.Default.Locator=ChannelIceGrid/Locator:tcp -p 10032 -h 192.168.2.128
  
  # Glacier2服务的ip和端口
  Glacier2.Client.Endpoints=tcp -p 10031 -h 192.168.2.128
  Glacier2.PermissionsVerifier=ChannelGlacier2/NullPermissionsVerifier
  Glacier2.SessionTimeout=30
  Glacier2.Client.ForwardContext=1
  Glacier2.Server.ForwardContext=1
  
  # 设置跟踪日志
  Glacier2.Client.Trace.Reject=1
  
  Ice.Warn.Connections=1
  Ice.MessageSizeMax=0
  ```

  > 说明：
  >
  > 此文件主要配置Glacier2服务的应用名称，IceGrid信息（IceGrid名称，ip和端口），IP地址和监听的端口；注意：配置项需要和config.json匹配；

- 配置计算任务config.json

  ```json
  {
    "TASK_ID": "task:0x0710cb",
    "PING_TIME": 1,
    "SEND_TIMEOUT": 10.0,
    "CONNECT_TIMEOUT": 10.0,
    "BUFFER_SIZE": 102400,
    "ROOT_CERT": "ca.pem",
    "NODE_INFO": [
      {
        "NODE_ID": "p0",
        "PUBLIC_IP": "192.168.2.128",
        "GRICER2": "VIA0",
        "ICEGRID": "GRID0",
        "CERT_DIR": "/home/luodahui/core/channel-sdk/ssl/openssl",
        "SERVER_CERT": "server.pem",
        "CLIENT_CERT": "client.pem",
        "PASSWORD": "123456"
      },
      {
        "NODE_ID": "p1",
        "PUBLIC_IP": "192.168.2.128",
        "GRICER2": "VIA0",
        "ICEGRID": "GRID0",
        "CERT_DIR": "/home/luodahui/core/channel-sdk/ssl/openssl",
        "SERVER_CERT": "server.pem",
        "CLIENT_CERT": "client.pem",
        "PASSWORD": "123456"
      },
      {
        "NODE_ID": "p2",
        "PUBLIC_IP": "192.168.2.128",
        "GRICER2": "VIA0",
        "ICEGRID": "GRID0",
        "CERT_DIR": "/home/luodahui/core/channel-sdk/ssl/openssl",
        "SERVER_CERT": "server.pem",
        "CLIENT_CERT": "client.pem",
        "PASSWORD": "123456"
      }
    ],
    "GRICER2_INFO": {
      "VIA0": {
        "APPNAME": "ChannelGlacier2",
        "IP": "192.168.2.128",
        "PORT": "10031"
      }
    },
    "ICE_GRID_INFO": {
      "GRID0": {
        "APPNAME": "ChannelIceGrid",
        "IP": "192.168.2.128",
        "PORT": "10032"
      }
    },
    "DATA_NODES": [
      "p0",
      "p1",
      "p2"
    ],
    "COMPUTATION_NODES": {
      "p0": "P0",
      "p1": "P1",
      "p2": "P2"
    },
    "RESULT_NODES": [
      "p0",
      "p1",
      "p2"
    ]
  }
  ```

  > 注意：此配置文件配置的Glacier2信息和IceGrid信息必须和config.glacier2，config.gridregistry配置的信息匹配，否则无法正常使用；

### 启动Glacier2

执行脚本：

```bash
./run_glacier2.sh
```

> 为了避免误操作将服务进程退出，Glacier2服务在后台运行，服务日志文件在当前的logs目录下；如果需要启用ssl，则执行：`./run_glacier2.sh ssl`。

### 启动ICEGrid

执行脚本：

```bash
./run_icegrid.sh
```

> 为了避免误操作将服务进程退出，IceGrid服务在后台运行，服务日志文件在当前的logs目录下；

### 查看ICE服务进程

执行脚本：

```bash
./status.sh
```

> 服务启动成功后可看到IceGrid和Glacier2服务进程状态和对应的监听日志进程。

### 退出ICE服务进程

执行脚本：

```bash
./killall.sh
```

> 此命令可将IceGrid和Glacier2服务进程退出，并退出监听日志进程。

## 测试

### 单独使用测试

进入`channel-sdk/test/python/one_to_one`目录，然后执行命令：

- 启动服务器

  ```bash
  python3 server.py
  ```

- 启动客户端

  ```bash
  python3 client.py
  ```

  

### Rosetta使用测试

如未安装sdk，先[安装sdk](#安装)；进入`channel-sdk/test/python/linear_saver_restore`目录，执行：

- 训练

  ```bash
  ./run_saver.sh
  ```

- 预测

  ```bash
  ./run_restore.sh
  ```


### 使用Ice测试

如未安装sdk，先[安装sdk](#安装)；进入`channel-sdk/test/python/linear_saver_restore`目录，执行前，请先按步骤启动[Ice服务](#ICE服务)；

执行：

```bash
./run_ice.sh
```

