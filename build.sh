#!/bin/bash
. ./function.sh

E="./"$(basename $0)
function show_usage() {
    echo "Welcome to channel-sdk."
    echo ""
    echo "USAGE:"
    echo "  $E COMMAND [OPTION]"
    echo ""
    echo "COMMANDS:"
    echo "  compile    compile the io channel libraries."
    echo "  install    install .whl"
    echo "  clean      clean workspace"
    echo "  $E COMMAND -h/--help"
    echo ""
}

function show_compile_usage() {
    echo "USAGE:"
    echo "  $E compile [OPTION]"
    echo "  e.g.: $E compile --use-alone"
    echo ""
    echo "OPTIONS:"
    echo "  -h --help                         Show this usage"
    echo "     --build-type                   [Release] One of [Release,Debug]"
    echo "     --server-type                  [ASYNC] One of [ASYNC,SYNC]"
    echo "     --client-type                  [SYNC] One of [ASYNC,SYNC]"
    echo "     --python-version               Python install version"
    echo "     --use-alone                    [OFF] The compiled version is used separately"
    echo "     --verbose                      [OFF] Display the detailed information of the compilation process, the value is a numeric type"
    echo ""
    echo "  The default options: --build-type Release --server-type ASYNC --client-type SYNC"
    echo ""
}

function show_clean_usage() {
    echo "USAGE:"
    echo "  $E clean"
    echo ""
}

function show_install_usage() {
    echo "USAGE:"
    echo "  $E install"
    echo ""
}

if [ $# -lt 1 ]; then
    show_usage
    exit 1
fi

cmd=${1}
if [ "${cmd}" = "compile" ]; then
    build_type=Release
    server_type=ASYNC
    client_type=SYNC
    python_version=$(python3 -c 'import sys;ver=sys.version_info;print(str(ver[0])+"."+str(ver[1]))')   
    use_alone=OFF

    ARGS=$(getopt -o "h" -l "help,build-type:,server-type:,client-type:,use-alone,python-version:,verbose:" -n "$0" -- "$@")
    eval set -- "${ARGS}"

    while true; do
        case "${1}" in
        -h | --help)
            show_compile_usage
            exit 0
            shift
            ;;
        --build-type)
            build_type=${2}
            shift 2
            ;;
        --server-type)
            server_type=${2}
            shift 2
            ;;
        --client-type)
            client_type=${2}
            shift 2
            ;;
        --python-version)
            # echo "python-version:${2}"
            python_version=${2}
            shift 2
            ;;
        --verbose)
            verbose=${2}
            shift 2
            ;;
        --use-alone)
            use_alone=ON
            shift
            ;;
        --)
            shift
            break
            ;;
        *)
            shift
            break
            ;;
        esac
    done
    echo -e "${BLUE}Ready to compile io channel${NC}"

    server_type=${server_type}"_SERVER"
    client_type=${client_type}"_CLIENT"
    run_compile
    run_compile_python $use_alone

elif [ "${cmd}" = "clean" ]; then
    ARGS=$(getopt -o "h" -l "help" -n "$0" -- "$@")
    eval set -- "${ARGS}"
    while true; do
        case "${1}" in
        -h | --help)
            show_clean_usage
            exit 0
            shift
            ;;
        --)
            shift
            break
            ;;
        *)
            shift
            break
            ;;
        esac
    done
    run_clean
    exit 0
elif [ "${cmd}" = "install" ]; then
    ARGS=$(getopt -o "h" -l "help" -n "$0" -- "$@")
    eval set -- "${ARGS}"
    while true; do
        case "${1}" in
        -h | --help)
            show_install_usage
            exit 0
            ;;
        --)
            shift
            break
            ;;
        *)
            shift
            break
            ;;
        esac
    done
    run_install_python
    exit 0
else
    show_usage    
fi
