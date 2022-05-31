# channel-sdk

RosettaNet网络层sdk

## 源码下载

```bash
git clone -b develop https://github.com/datumtechs/datum-network-channel-sdk.git
```

## 源码编译

说明：如非源码编译方式，可不执行下面的操作步骤；

### 环境预安装

```bash
sudo apt-get install build-essential libtool libgflags-dev clang libc++-dev
```

### 编译

```bash
cd datum-network-channel-sdk && ./build.sh compile
```

>**编译选项说明：**
>
>- --build-type: 编译版本，包括Release（默认）和Debug；
>- --server-type：服务器类型，包括ASYNC（默认）和SYNC，ASYNC表示异步服务器，SYNC表示同步服务器；
>- --client-type：客户端类型，包括ASYNC和SYNC（默认） ，ASYNC表示异步客户端，SYNC表示同步客户端；
>- --use-buffer：是否使用循环缓存存储数据，如果是ON表示是，OFF表示否（默认为否）；
>- --package-ice-via：是否打包IceGrid和Glacier2相关via文件，如果是ON表示是，OFF表示否（默认为否）；
>- --static-call：是否使用静态调用方式，如果是ON表示是，OFF表示否（默认为否，即表示动态调用）；
>- --verbose：是否显示编译详情，默认否 ；
>- --ssl-type：通讯通道使用证书类型；0：不使用证书（默认），1：使用openssl库，2：使用gmssl库；
>- --python-version: 指定python版本打包; 默认为当前系统python3对应的版本, ubuntu18.04默认为：python3.6;
>
>**例子：**
>
>```bash
>./build.sh compile --build-type=Debug --static-call --use-buffer --server-type=ASYNC --package-ice-via --verbose=1 --ssl-type=2 --python-version=3.7
>```
>
>编译成功后在dist目录下，生成`whl`安装文件；



## 安装

```bash
./build.sh install
```

> 将dist目录下的whl安装文件安装到python的安装目录下；安装文件主要包括：
>
> - channel sdk动态库；
> - ice相关的依赖库；
> - ice的二进制icegridregistry，glacier2router和相关的配置文件；
> - 执行和退出ice二进制服务的脚本文件；



## 删除卸载

```bash
./build.sh clean
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
    "CONNECT_SYNC": false,
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
    ],
    "POLICY": {
      "p0": ["p1","p2"],
      "p1": ["p0","p2"],
      "p2": ["p0","p1"]
    }
  }
  ```

  > 注意：此配置文件配置的Glacier2信息和IceGrid信息必须和config.glacier2，config.gridregistry配置的信息匹配，否则无法正常使用；其中`PUBLIC_IP`表示通过代理转发消息时，允许被Glacier2服务访问的节点服务器IP地址（如果在同一台机器上，可设置为127.0.0.1）；POLICY为连接策略配置，当不配置此选项或为空时，表示全连接; 连接策略配置中的字典key为客户端节点ID，字典value为服务端节点ID列表。

  
### 启动Glacier2

执行脚本：

```bash
./run_glacier2.sh &
```

> 为了避免误操作将服务进程退出，Glacier2服务在后台运行，服务日志文件在当前的logs目录下；如果需要启用ssl，则执行：`./run_glacier2.sh ssl &`。

### 启动ICEGrid

执行脚本：

```bash
./run_icegrid.sh &
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
./stop.sh
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

### PSI使用测试

如未安装sdk，先[安装sdk](#安装)；进入`channel-sdk/test/python/psi_demo`目录，执行：
```bash
./run_demo_extio.sh
```

### 使用Ice测试

如未安装sdk，先[安装sdk](#安装)；进入`channel-sdk/test/python/linear_saver_restore`目录，执行前，请先按步骤启动[Ice服务](#ICE服务)；

执行：

```bash
./run_ice.sh
```

## Docker使用

channel-sdk支持Docker化编译部署测试，由于生产环境上，Ice Via服务通常单独部署，所以channel-sdk和Ice Via的镜像分开打包；其中项目使用docker-compose工具对宿主机上的容器和镜像进行管理，相关工具安装，可执行命令：

```bash
./install_docker.sh
```

相关docker-compose配置可参考docker-compose.yml文件，如：

```yaml
# yaml 配置
version: '3.6'
services:
  build_sdk:
    image: "luodahui/channel-sdk:v2.0.3"
    build:
      network: "host"
      context: .
      args: 
        # "build_args": "--package-ice-via"
        "install_flag": 1
      dockerfile: Dockerfile
  build_ice_via:
    image: "luodahui/ice_via:v2.0.3"
    build:
      network: "host"
      context: .
      dockerfile: IceDockerfile
  test_run_server:
    image: "luodahui/channel-sdk:v2.0.3"
    container_name: test_server
    restart: always
    # env_file:
    #   - p1.env
    volumes:
      - ./test/python/docker/p1_server_config.json:/ChannelSDK/test/python/one_to_one.json
    network_mode: "host"
    # ports:
    #   - ""192.168.2.128:10001:10001"
    command: python3 /ChannelSDK/test/python/server.py
  test_run_client:
    image: "luodahui/channel-sdk:v2.0.3"
    container_name: test_client
    restart: always
    # env_file:
    #   - p0.env
    volumes:
      - ./test/python/docker/p0_client_config.json:/ChannelSDK/test/python/one_to_one.json
    network_mode: "host"
    # ports:
    #   - "192.168.2.128:10000:10000"
    command: python3 /ChannelSDK/test/python/client.py
  test_run_glacier2:
    container_name: test_glacier2
    restart: always
    network_mode: "host"
    image: "luodahui/ice_via:v2.0.3"
    volumes:
      - ./test/python/docker/config/config.glacier2:/IceVia/config/config.glacier2
    command: /IceVia/bin/run_glacier2.sh
  test_run_ice_grid:
    container_name: test_ice_grid
    restart: always
    network_mode: "host"
    volumes:
      - ./test/python/docker/config/config.gridregistry:/IceVia/config/config.gridregistry
    image: "luodahui/ice_via:v2.0.3"
    command: /IceVia/bin/run_icegrid.sh
  test_run_server_via:
    image: "luodahui/channel-sdk:v2.0.3"
    container_name: test_server_via
    restart: always
    network_mode: "host"
    command: python3 /ChannelSDK/test/python/server_via.py
    depends_on:
      - test_run_glacier2
      - test_run_ice_grid
  test_run_client_via:
    image: "luodahui/channel-sdk:v2.0.3"
    container_name: test_client_via
    restart: always
    network_mode: "host"
    command: python3 /ChannelSDK/test/python/client_via.py
  

```

> 说明：
>
> build_sdk：打包channel sdk镜像的命令参数配置；
>
> build_ice_via：打包ice via镜像的命令参数配置；
>
> test_run_server：测试运行容器中的服务器；
>
> test_run_client：测试运行容器中的客户端；
>
> test_run_glacier2：测试运行容器中的glacier2服务；
>
> test_run_ice_grid：测试运行容器中的ice_grid服务；
>
> test_run_server_via：测试运行容器中带ice via的服务器；
>
> test_run_client_via：测试运行容器中带ice via的客户端；

运行命令如下：

### 打包sdk镜像

```bash
docker-compose up --build build_sdk
```

### 打包Ice Via镜像

```bash
docker-compose up --build build_ice_via
```

### 普通C/S测试

运行容器，进行客户端/服务器模式测试，命令如下：

- 启动服务器

  ```bash
  docker-compose run test_run_server
  ```

- 启动客户端

  ```bash
  docker-compose run test_run_client
  ```

### 代理C/S测试

运行容器，进行代理服务的客户端/服务器模式测试，命令如下：

- 启动服务器

  ```bash
  docker-compose run test_run_server_via
  ```

  > 配置文件中指定test_run_server_via容器服务依赖于test_run_glacier2和test_run_ice_grid服务，所以启动服务端的容器服务前，会自动先启动ice via服务的容器；如果需要手动启动ice via服务，可执行如下命令：
  >
  > ```bash
  > # 后台启动glacier2
  > docker-compose run -d test_run_glacier2
  > # 后台启动ice grid
  > docker-compose run -d test_run_ice_grid
  > ```

- 启动客户端

  ```bash
  docker-compose run test_run_client_via
  ```

  