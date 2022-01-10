#!/bin/bash

mkdir -p deploy/lmdb/registry

curdir=$(pwd)

datefmt=$(date +'%Y%m%d%H%M%S')
mkdir -p ${curdir}/logs
log_path=${curdir}/logs

export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:$curdir/..
./icegridregistry --Ice.Config=config.gridregistry >> ${log_path}/icegrid-${datefmt}.log 2>&1 & 
