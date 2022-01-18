#!/bin/bash

curdir=$(pwd)

config_file=config.glacier2
if [ $# -lt 2 ]; then
    if [ "${1}" = "ssl" ]; then
        config_file=config.ssl.glacier2
    fi
fi

datefmt=$(date +'%Y%m%d%H%M%S')
mkdir -p ${curdir}/logs
log_path=${curdir}/logs

export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:$curdir/..
echo "current log file: ${log_path}/glacier2-${datefmt}.log"
./glacier2router --Ice.Config=${config_file} >>${log_path}/glacier2-${datefmt}.log 2>&1 & 
tail -f ${log_path}/glacier2-${datefmt}.log &

exit 0
