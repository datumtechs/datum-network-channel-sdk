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
    echo "  ice        compile the ice files."
    echo "  install    install .whl"
    echo "  clean      clean workspace"
    echo "  $E COMMAND -h/--help"
    echo ""
}

function show_compile_usage() {
    echo "USAGE:"
    echo "  $E compile [OPTION]"
    echo "  e.g.: $E compile"
    echo ""
    echo "OPTIONS:"
    echo "  -h --help                         Show this usage"
    echo "     --build-type                   [Release] One of [Release,Debug]"
    echo "     --server-type                  [ASYNC] One of [ASYNC,SYNC]"
    echo "     --use-buffer                   [ON] Whether to use circular buffer to save data. The default value is no"
    echo "     --multi-locks                  [OFF] Asynchronous server parameters:Use multi-lock switches, and nodeid mapping"
    echo "     --thread-count                 [4] Number of threads to process asynchronous server events, default to the number of CPU cores available "
    echo "     --client-type                  [SYNC] One of [ASYNC,SYNC]"
    echo "     --python-version               Python install version"
    echo "     --static-call                  [OFF] Static call mode of RPC interface, default:OFF, which indicates dynamic invocation"
    echo "     --ssl-type                     [0] SSL Type, 0: SSL is not used; 1: using openssl;  2: using GMSSL"
    echo "     --package-ice-via              [OFF] Whether to package and install IceGrid and Glacier2 related binaries and configuration files."
    echo "     --verbose                      [OFF] Display the detailed information of the compilation process, the value is a numeric type"
    echo ""
    echo "  The default options: --build-type Release --server-type ASYNC --client-type SYNC"
    echo ""
}

function show_ice_usage() {
    echo "USAGE:"
    echo "  $E ice"
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
    server_type=SYNC
    client_type=SYNC
    python_version=$(python3 -c 'import sys;ver=sys.version_info;print(str(ver[0])+"."+str(ver[1]))')   
    ssl_type=0
    use_buffer=OFF
    static_call=OFF
    thread_count=1
    multi_locks=OFF
    package_ice_via=OFF
    ARGS=$(getopt -o "h" -l "help,build-type:,server-type:,client-type:,ssl-type:,static-call,
        use-buffer,thread-count:,multi-locks,package-ice-via,python-version:,verbose:" -n "$0" -- "$@")
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
            python_version=${2}
            shift 2
            ;;
        --verbose)
            verbose=${2}
            shift 2
            ;;
        --ssl-type)
            ssl_type=${2}
            shift 2
            ;;
        --use-buffer)
            use_buffer=ON
            shift
            ;;
        --static-call)
            static_call=ON
            shift
            ;;
        --thread-count)
            thread_count=${2}
            shift 2
            ;;
        --multi-locks)
            multi_locks=ON
            shift
            ;;
        --package-ice-via)
            package_ice_via=ON
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
    run_compile_python $package_ice_via $python_version
elif [ "${cmd}" = "ice" ]; then
    ARGS=$(getopt -o "h" -l "help" -n "$0" -- "$@")
    eval set -- "${ARGS}"
    while true; do
        case "${1}" in
        -h | --help)
            show_ice_usage
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
    run_compile_ice
    exit 0
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
