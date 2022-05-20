# 基础镜像，基于gcc镜像构建--编译阶段
FROM ubuntu:18.04
# 作者
MAINTAINER luodahui

#更新为国内的镜像仓库，因为从默认官源拉取实在太慢了
RUN cp /etc/apt/sources.list /etc/apt/sources.list.bak
COPY ./apt_src.ubuntu18.04 /etc/apt/sources.list
# debconf: unable to initialize frontend: Dialog
ENV DEBIAN_FRONTEND noninteractive

RUN apt-get update
RUN apt-get install -y --no-install-recommends \ 
    apt-utils \
    build-essential \
    gcc \
    g++ \
    vim \
    make \
    cmake \ 
    libssl1.0.0 \
    libc-dev \
    libbz2-dev && apt-get autoremove

# python/pip
RUN apt-get install -y --no-install-recommends \
    python3.7 \
    python3.7-dev \ 
    python3-distutils \
    python3-pip && apt-get autoremove

RUN cd /usr/bin && ln -sf python3.7 python && ln -sf python3.7 python3 \
    && ln -s pip3 pip \
    && python -m pip install --upgrade pip
# wheel：打包whl文件命令
RUN pip3 install setuptools==57.5.0 wheel -i https://mirrors.aliyun.com/pypi/simple/

RUN mkdir -p /ChannelSDK/python/channel_sdk
# 保存ice动态库，微服务配置文件以及二进制文件目录
RUN mkdir /ChannelSDK/python/channel_sdk/bin
RUN mkdir /ChannelSDK/python/channel_sdk/lib
RUN mkdir /ChannelSDK/python/channel_sdk/config
WORKDIR /ChannelSDK
COPY cmake /ChannelSDK/cmake
COPY core /ChannelSDK/core
COPY python/channel_sdk/__init__.py ./python/channel_sdk
COPY third_party /ChannelSDK/third_party
COPY test/python/one_to_one /ChannelSDK/test/python/one_to_one
COPY build.sh .
COPY clean.sh .
COPY function.sh .
COPY setup.py .
COPY CMakeLists.txt .
COPY MANIFEST.in .
# 编译生成whl, 然后安装
RUN ./build.sh clean && ./build.sh compile --package-ice-via && ./build.sh install
# 删除源码，依赖库等文件

