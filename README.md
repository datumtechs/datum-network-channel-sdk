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
mkdir channel-sdk/build
cmake ..
make
```

> 编译成功后在channel-sdk/lib下生成so文件；



