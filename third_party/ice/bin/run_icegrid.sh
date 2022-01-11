#!/bin/bash

mkdir -p deploy/lmdb/registry

curdir=$(pwd)

datefmt=$(date +'%Y%m%d%H%M%S')
mkdir -p ${curdir}/logs
log_path=${curdir}/logs

export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:$curdir/..
echo "current log file: ${log_path}/icegrid-${datefmt}.log"
./icegridregistry --Ice.Config=config.gridregistry >> ${log_path}/icegrid-${datefmt}.log 2>&1 & 
tail -f ${log_path}/icegrid-${datefmt}.log &
