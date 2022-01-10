#!/bin/bash

curdir=$(pwd)

datefmt=$(date +'%Y%m%d%H%M%S')
mkdir -p ${curdir}/logs
log_path=${curdir}/logs
# 配置环境变量
export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:$curdir/..

./glacier2router --Ice.Config=config.glacier2 >>${log_path}/glacier2-${datefmt}.log 2>&1 & 
