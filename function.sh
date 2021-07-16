#!/bin/bash
curdir=$(pwd)
builddir=${curdir}/build
if [ ! -d $builddir ]; then
  mkdir $builddir
fi

pv=$(python3 -c 'import sys; print(sys.version_info[0])')
pip_cmd=pip
if [ "$pv" == '3' ]; then
  pip_cmd=pip3
fi

RED='\033[0;31m'
GREEN='\033[0;32m'
BLUE='\033[0;34m'
PINK='\033[0;35m'
NC='\033[0m' # No Color

function run_compile() {
    echo -e "${GREEN}Start compiling.${NC}"
    cd ${builddir}
    cmake .. -DCMAKE_INSTALL_PREFIX=.install -DCMAKE_PREFIX_PATH=${builddir}  \
        -DUSE_ALONE=${use_alone} \
        -DCMAKE_BUILD_TYPE=${build_type} \
        -DSERVER_TYPE=${server_type} \
        -DCLIENT_TYPE=${client_type}
        # -DPYBIND11_PYTHON_VERSION=${python_version}

    if [ -z ${verbose} ];then
        make -j4
    else
        make -j4 VERBOSE=${verbose}
    fi

    cd ${curdir}
    echo -e "${GREEN}Compile executed successfully.${NC}"
}

function run_clean() {
    echo -e "${GREEN}Start cleaning.${NC}"
    ./clean.sh
    echo -e "${GREEN}Cleaned up successfully.${NC}"
}

function run_compile_python() {
  echo -e "stage run_compile_python."
  cd ${curdir}
  if [ "$USER" == "root" ]; then
    ${pip_cmd} uninstall io_channel -y
  else
    python3 -m pip uninstall io_channe -y # for the current user
  fi

  export USE_ALONE=$1 && python3 setup.py build_ext
  export USE_ALONE=$1 && python3 setup.py bdist_wheel

  cd ${curdir}
  echo -e "${GREEN}run stage run_compile_python ok.${NC}"
}

function run_install_python() {
    echo -e "${GREEN}Start install.${NC}"
    cd ${curdir}

    if [ "$USER" == "root" ]; then
        ${pip_cmd} install dist/*.whl
    else
        ${pip_cmd} install dist/*.whl --user
    fi

    cd ${curdir}
    echo -e "${GREEN}Install successfully.${NC}"
}