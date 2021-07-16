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
> - --build-type: 编译版本，包括Release（默认）和Debug；
>
> - --server-type：服务器类型，包括ASYNC（默认）和SYNC，ASYNC表示异步服务器，SYNC表示同步服务器；
>
> - --client-type：客户端类型，包括ASYNC和SYNC（默认） ，ASYNC表示异步客户端，SYNC表示同步客户端；
>
> - --use-alone：是否单独使用，如果是ON表示是，OFF表示否（默认，供rosetta使用）；
>
> - --verbose：是否显示编译详情，默认否；
>
>
>
>**例子：**
>
> ```bash
>./build.sh compile --build-type=Debug --server-type=ASYNC --verbose=1
> ```



## 安装

```bash
cd channel-sdk && ./build.sh install
```



## 删除卸载

```bash
cd channel-sdk && ./build.sh clean
```



## 测试

### 单独使用测试

使用源码编译方式，且需要带`--use-alone`参数进行编译；将`channel-sdk/test/python`下的例子拷贝到channel-sdk目录下，进行测试：

- 启动服务器

  ```bash
  python3 server.py
  ```

- 启动客户端

  ```bash
  python3 client.py
  ```

  

### Rosetta使用测试

如已安装单独使用的sdk，需先卸载，然后再[安装sdk](#安装)；进入`channel-sdk/test/python/linear_saver_restore`目录，执行：

- 训练

  ```bash
  ./run_saver.sh
  ```

- 预测

  ```bash
  ./run_restore.sh
  ```

  



