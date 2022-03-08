#!/bin/bash

script_dir=$(dirname $0)
bin_dir=$script_dir
lib_dir=$script_dir/../lib
config_dir=$script_dir/../config
config_file=${config_dir}/config.gridregistry
data_dir=deploy/lmdb/registry
mkdir -p $data_dir
log_dir=${script_dir}/../logs
datefmt=$(date +'%Y%m%d%H%M%S')
mkdir -p ${log_dir}

export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:$lib_dir
echo "current log file: ${log_dir}/icegrid-${datefmt}.log"
$bin_dir/icegridregistry --Ice.Config=$config_file >> ${log_dir}/icegrid-${datefmt}.log 2>&1 & 
tail -f ${log_dir}/icegrid-${datefmt}.log &
