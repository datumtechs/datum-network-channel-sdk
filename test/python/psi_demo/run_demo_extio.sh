#!/bin/bash
# set -x

times=${1:-1} # how many times will run

curdir=$(pwd)
bindir=${curdir}/build/bin
mkdir -p ${bindir}
# ================================ COMMOM

#
cp -rf psi_demo_extio.py ${bindir}/
cp -af config.json ${bindir}/
cp -af kill.sh ${bindir}/
cp -af gen_fake_data ${bindir}/

# almost the same with XSDK/psi/run_demo_ex_local.sh
# ####################################
cd ${bindir}
export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:/usr/local/lib/python3.7/dist-packages/latticex
sudo bash ./kill.sh
datadir=/tmp/psidata
rm -rf log out core*
mkdir -p log out data ${datadir}

sender_bs=2000000
receiver_bs=2000000
finder_bs=6000000
threadnum=1
# ####################################
cmd='python3 ./psi_demo_extio.py'
echo $cmd

function run_demo() {
  echo "run demo============="
  records=$1
  ubal=$2
  ptype=$3
  # ps -ef | grep psi
  sudo bash ./kill.sh
  # ########################
  meta_file=${datadir}/${records}-meta.txt
  sender_input=${datadir}/${records}-sender_ids_labels.csv
  if [[ "${ptype}" =~ "Basic" ]]; then
    sender_input=${datadir}/${records}-sender_ids.csv
  fi
  receiver_input=${datadir}/${records}-receiver_ids_${ubal}.csv
  receiver_output=out/${ubal}-${ptype}-${records}-receiver.csv
  rm -rf ${receiver_output}*
  # ########################
  if [ ! -f "${sender_input}" ]; then
    cd ${datadir}/
    cp -f ${bindir}/gen_fake_data ${datadir}/
    echo "gen fake data"
    ./gen_fake_data ${records} 3 6
    sleep 1
  fi
  # ########################
  cd ${bindir}
  echo "run psi type(${ubal} ${ptype}) records(${records})"
  rm -rf log/${ubal}-${ptype}-${records}-*-Backend*
  $cmd --node_id=P0 --ioconfig=config.json --threadnum=${threadnum} \
    --sender_bs=${sender_bs} --receiver_bs=${receiver_bs} --finder_bs=${finder_bs} \
    --psi_type=${ptype} --log_prefix=${ubal}-${ptype}-${records}-sender-Backend --run_times=1 \
    --input=${sender_input} >log/${ubal}-${ptype}-${records}-sender-Console.log 2>&1 &
  $cmd --node_id=P1 --ioconfig=config.json --threadnum=${threadnum} \
    --sender_bs=${sender_bs} --receiver_bs=${receiver_bs} --finder_bs=${finder_bs} \
    --psi_type=${ptype} --log_prefix=${ubal}-${ptype}-${records}-receiver-Backend --run_times=1 \
    --input=${receiver_input} --output=${receiver_output} >log/${ubal}-${ptype}-${records}-receiver-Console.log 2>&1
  # ########################
  wait
  sleep 1
  # cat ${meta_file}
  # grep -E "psistats|\+----------" log/${ubal}-${ptype}-${records}-sender-Console.log
  grep -E "psistats|\+-------------------" log/${ubal}-${ptype}-${records}-sender-Console.log
  grep -E "psistats|\+-------------------" log/${ubal}-${ptype}-${records}-receiver-Console.log
  grep '"elapse":' log/${ubal}-${ptype}-${records}-sender-Console.log
  grep '"elapse":' log/${ubal}-${ptype}-${records}-receiver-Console.log

  wc ${receiver_output}*
  n=$(wc ${receiver_output}* -l | head -n 1 | awk '{print $1}')
  repeat_num=$(cat ${meta_file} | grep repeat_num | grep \(${ubal}\) | awk '{print $6}')
  if [[ "$n" == "" ]] || [[ $n -ne ${repeat_num} ]]; then
    echo "n=$n !=repeat_num(${repeat_num})"
    exit 1
  fi
  sleep 1
}

# all
us=(bal unbal)
rs=(100000 1000000 10000000 100000000)
ss=(
  T_V1_Basic_SECP
  T_V1_Basic_SECP_OPT

  T_V2_Basic_SECP
  T_V2_Basic_SECP_OPT
  T_V2_Labeled_SECP
  T_V2_Labeled_SECP_OPT

  T_V1_Basic_GLS254
  T_V1_Basic_GLS254_OPT

  T_V2_Basic_GLS254
  T_V2_Basic_GLS254_OPT
  T_V2_Labeled_GLS254
  T_V2_Labeled_GLS254_OPT
)

# special
rs=(50000000)
rs=(30000000)
rs=(10000000)
rs=(1000000)
rs=(100000)
rs=(10000)
rs=(1000)
rs=(100)
us=(bal)
# one
ss=(T_V2_Labeled_SECP_OPT)
# all
ss=(T_V1_Basic_SECP T_V1_Basic_SECP_OPT T_V1_Basic_GLS254 T_V1_Basic_GLS254_OPT
  T_V2_Basic_SECP T_V2_Basic_SECP_OPT T_V2_Basic_GLS254 T_V2_Basic_GLS254_OPT
  T_V2_Labeled_SECP T_V2_Labeled_SECP_OPT T_V2_Labeled_GLS254 T_V2_Labeled_GLS254_OPT)
# nopt
ss=(
  T_V1_Basic_SECP
  T_V2_Basic_SECP
  T_V2_Labeled_SECP
  T_V1_Basic_GLS254
  T_V2_Basic_GLS254
  T_V2_Labeled_GLS254
)
# opt
ss=(
  T_V1_Basic_SECP_OPT
  T_V2_Basic_SECP_OPT
  T_V2_Labeled_SECP_OPT
  T_V1_Basic_GLS254_OPT
  T_V2_Basic_GLS254_OPT
  T_V2_Labeled_GLS254_OPT
)
# secp
ss=(T_V1_Basic_SECP T_V1_Basic_SECP_OPT T_V2_Basic_SECP T_V2_Basic_SECP_OPT T_V2_Labeled_SECP T_V2_Labeled_SECP_OPT)
# gls254
ss=(T_V1_Basic_GLS254 T_V1_Basic_GLS254_OPT T_V2_Basic_GLS254 T_V2_Basic_GLS254_OPT T_V2_Labeled_GLS254 T_V2_Labeled_GLS254_OPT)
# all
ss=(T_V1_Basic_SECP T_V1_Basic_SECP_OPT T_V1_Basic_GLS254 T_V1_Basic_GLS254_OPT
  T_V2_Basic_SECP T_V2_Basic_SECP_OPT T_V2_Basic_GLS254 T_V2_Basic_GLS254_OPT
  T_V2_Labeled_SECP T_V2_Labeled_SECP_OPT T_V2_Labeled_GLS254 T_V2_Labeled_GLS254_OPT)

ss=(T_V1_Basic_SECP)
for ((i = 0; i < ${times}; i++)); do
  for r in ${rs[@]}; do
    for u in ${us[@]}; do
      for s in ${ss[@]}; do
        echo -e "\n${i} ${r} ${u} ${s}\n"
        run_demo ${r} ${u} ${s}
      done
    done
  done
  cd ${bindir}
  wc out/*
done

cd ${curdir}
