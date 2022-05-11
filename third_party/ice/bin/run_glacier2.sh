#!/bin/bash

#curdir=$(pwd)
script_dir=$(dirname $0)
bin_dir=$script_dir
lib_dir=$script_dir/../lib
config_dir=$script_dir/../config
config_file=${config_dir}/config.glacier2

if [ $# -lt 2 ]; then
    if [ "${1}" = "ssl" ]; then
        config_file=${config_dir}/config.ssl.glacier2
    fi
fi

log_dir=${script_dir}/../logs
datefmt=$(date +'%Y%m%d%H%M%S')
mkdir -p ${log_dir}

export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:$lib_dir
echo "current log file: ${log_dir}/glacier2-${datefmt}.log"
$bin_dir/glacier2router --Ice.Config=${config_file} >>${log_dir}/glacier2-${datefmt}.log 2>&1 & 
tail -f ${log_dir}/glacier2-${datefmt}.log &

exit 0