cmake_minimum_required(VERSION 3.5.1)

SET(LIBNAME extio)
project(${LIBNAME})

include(./cmake/common.cmake)
include(./cmake/SetCommon.cmake)
# add_definitions(-std=c++11)

#list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")


#file(MAKE_DIRECTORY ${lib_path})
# set(LIBRARY_OUTPUT_PATH ${CMAKE_CURRENT_SOURCE_DIR}/lib)

# Generate Proto file
# get_filename_component(io_proto "./protos/io_channel.proto" ABSOLUTE)
# get_filename_component(io_proto_path "${io_proto}" PATH)
# file(MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/protos)  
# set(out_proto_dirs "${CMAKE_CURRENT_BINARY_DIR}/protos")
# set(io_proto_srcs "${out_proto_dirs}/io_channel.pb.cc")
# set(io_proto_hdrs "${out_proto_dirs}/io_channel.pb.h")
# set(io_grpc_srcs "${out_proto_dirs}/io_channel.grpc.pb.cc")
# set(io_grpc_hdrs "${out_proto_dirs}/io_channel.grpc.pb.h")
# add_custom_command(
#       OUTPUT "${io_proto_srcs}" "${io_proto_hdrs}" "${io_grpc_srcs}" "${io_grpc_hdrs}"
#       COMMAND ${_PROTOBUF_PROTOC}
#       ARGS --grpc_out "${out_proto_dirs}"
#         --cpp_out "${out_proto_dirs}"
#         -I "${io_proto_path}"
#         --plugin=protoc-gen-grpc="${_GRPC_CPP_PLUGIN_EXECUTABLE}"
#         "${io_proto}"
#       DEPENDS "${io_proto}")

# # Include generated *.pb.h files
# include_directories("${CMAKE_CURRENT_BINARY_DIR}")
# add_library(io_grpc_proto
#   ${io_grpc_srcs} ${io_grpc_hdrs} ${io_proto_srcs} ${io_proto_hdrs})

# target_link_libraries(io_grpc_proto
#   ${_REFLECTION}
#   ${_GRPC_GRPCPP}
#   ${_PROTOBUF_LIBPROTOBUF})

# option
option(USE_SSL "" OFF)
message(STATUS "[USE_SSL]: ${USE_SSL}")
IF(USE_SSL)
    add_definitions(-DUSE_SSL=1)
ENDIF()

option(USE_ALONE "" OFF)
message(STATUS "[USE_ALONE]: ${USE_ALONE}")
IF(USE_ALONE)
    add_definitions(-DUSE_ALONE=1)
ENDIF()

option(SERVER_TYPE "" OFF)
message(STATUS "[SERVER_TYPE]: ${SERVER_TYPE}")
IF(SERVER_TYPE)
    add_definitions(-D${SERVER_TYPE}=1)
ENDIF()

option(USE_CACHE "" OFF)
message(STATUS "[USE_CACHE]: ${USE_CACHE}")
IF(USE_CACHE)
    add_definitions(-DUSE_CACHE=1)
ENDIF()

option(MULTI_LOCKS "" OFF)
message(STATUS "[MULTI_LOCKS]: ${MULTI_LOCKS}")
IF(MULTI_LOCKS)
    add_definitions(-DMULTI_LOCKS=1)
ENDIF()

option(THREAD_COUNT "" OFF)
message(STATUS "[THREAD_COUNT]: ${THREAD_COUNT}")
IF(THREAD_COUNT)
    add_definitions(-DTHREAD_COUNT=${THREAD_COUNT})
ENDIF()

option(CLIENT_TYPE "" OFF)
message(STATUS "[CLIENT_TYPE]: ${CLIENT_TYPE}")
IF(CLIENT_TYPE)
    add_definitions(-D${CLIENT_TYPE}=1)
ENDIF()


# third party
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/third_party/rapidjson/include)

# threads
find_package(Threads REQUIRED)

SET(ADD_LINK_LIB_FLAGS "-Wl,--rpath=$ORIGIN../:$ORIGIN")
SET(CMAKE_EXE_LINKER_FLAGS  "${CMAKE_EXE_LINKER_FLAGS} ${ADD_LINK_LIB_FLAGS}")


message(STATUS "_REFLECTION: ${_REFLECTION}")
message(STATUS "_GRPC_GRPCPP: ${_GRPC_GRPCPP}")
message(STATUS "_PROTOBUF_LIBPROTOBUF: ${_PROTOBUF_LIBPROTOBUF}")

# link_libraries(${_REFLECTION} ${_GRPC_GRPCPP} ${_PROTOBUF_LIBPROTOBUF})
# libraries
file(GLOB_RECURSE srcs core/src/*.cc)
add_library(${LIBNAME} SHARED ${srcs})
target_include_directories(${LIBNAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/core/include)
target_link_libraries(${LIBNAME} PUBLIC pthread ${_REFLECTION} ${_GRPC_GRPCPP} ${_PROTOBUF_LIBPROTOBUF})

# set_target_properties(${LIBNAME} PROPERTIES FOLDER "extio"
#     APPEND_STRING PROPERTY LINK_FLAGS " ${ADD_LINK_LIB_FLAGS}")

# targets pybind11

# add_subdirectory("./third_party/pybind11/" pybind11)
# pybind11_add_module(io_channel core/pybind11/io_channel.cc)
# link_libraries(${LIBNAME})
# target_link_libraries(io_channel PRIVATE ${LIBNAME})
