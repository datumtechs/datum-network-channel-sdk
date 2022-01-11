#!/bin/bash

curdir=$(pwd)

datefmt=$(date +'%Y%m%d%H%M%S')
mkdir -p ${curdir}/logs
log_path=${curdir}/logs

export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:$curdir/..
echo "current log file: ${log_path}/glacier2-${datefmt}.log"
./glacier2router --Ice.Config=config.glacier2 >>${log_path}/glacier2-${datefmt}.log 2>&1 & 
tail -f ${log_path}/glacier2-${datefmt}.log &

exit 0
