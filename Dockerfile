# 基础镜像，基于gcc镜像构建--编译阶段
FROM ubuntu:18.04 AS bulider
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
    gcc \
    g++ \
    build-essential \
    vim \
    make \
    cmake \ 
    libssl1.0.0 \
    libc-dev \
    # net-tools \
    # iputils-ping \
    libbz2-dev --fix-missing && apt-get autoremove

# python/pip
RUN apt-get install -y --no-install-recommends \
    python3.7 \
    python3.7-dev \ 
    python3-distutils \
    python3-pip && apt-get autoremove

RUN cd /usr/bin && ln -sf python3.7 python && ln -sf python3.7 python3 \
    && ln -s pip3 pip \
    && python -m pip install --upgrade pip -i https://mirrors.aliyun.com/pypi/simple/
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
COPY build.sh .
COPY clean.sh .
COPY function.sh .
COPY setup.py .
COPY CMakeLists.txt .
COPY MANIFEST.in .
# 编译生成whl
ARG build_args=""
ENV compile_params=${build_args}
RUN echo "compile_params============${compile_params}"
RUN ./build.sh clean && ./build.sh compile ${compile_params}

# 运行环境
FROM ubuntu:18.04 AS runner
RUN cp /etc/apt/sources.list /etc/apt/sources.list.bak
COPY ./apt_src.ubuntu18.04 /etc/apt/sources.list
# debconf: unable to initialize frontend: Dialog
ENV DEBIAN_FRONTEND noninteractive
RUN apt-get update
RUN apt-get install -y --no-install-recommends \ 
    libssl1.0.0 \
    && apt-get autoremove

# python/pip
RUN apt-get install -y --no-install-recommends \
    python3.7 \
    python3.7-dev \ 
    python3-distutils \
    python3-pip && apt-get autoremove

RUN cd /usr/bin && ln -sf python3.7 python && ln -sf python3.7 python3 \
    && ln -s pip3 pip \
    && python -m pip install --upgrade pip -i https://mirrors.aliyun.com/pypi/simple/
# wheel：打包whl文件命令
RUN pip3 install setuptools==57.5.0 wheel -i https://mirrors.aliyun.com/pypi/simple/

WORKDIR /ChannelSDK
# # 复制编译阶段编译出来的运行文件到目标目录
COPY --from=bulider /ChannelSDK/dist /ChannelSDK/dist
COPY test/python/docker /ChannelSDK/test/python/
COPY build.sh .
COPY clean.sh .
COPY function.sh .
# 是否安装
ARG install_flag=0
RUN if [ ${install_flag} = 1 ] ; then ./build.sh install; fi
RUN rm -rf build ice_via/bin/deploy ice_via/logs *.*
