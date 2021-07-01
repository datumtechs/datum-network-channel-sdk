cmake_minimum_required(VERSION 3.5.1)

SET(LIBNAME external_io)
project(${LIBNAME})

include(./cmake/common.cmake)
add_definitions(-std=c++11)

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


# third party
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/third_party/rapidjson/include)

# threads
find_package(Threads REQUIRED)

SET(ADD_LINK_LIB_FLAGS "-Wl,--rpath=$ORIGIN../:$ORIGIN")
SET(CMAKE_EXE_LINKER_FLAGS  "${CMAKE_EXE_LINKER_FLAGS} ${ADD_LINK_LIB_FLAGS}")


message(STATUS "_REFLECTION: ${_REFLECTION}")
message(STATUS "_GRPC_GRPCPP: ${_GRPC_GRPCPP}")
message(STATUS "_PROTOBUF_LIBPROTOBUF: ${_PROTOBUF_LIBPROTOBUF}")

link_libraries(${_REFLECTION} ${_GRPC_GRPCPP} ${_PROTOBUF_LIBPROTOBUF})
# libraries
file(GLOB_RECURSE srcs core/src/*.cc)
add_library(${LIBNAME} SHARED ${srcs})
target_include_directories(${LIBNAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/core/include)
target_link_libraries(${LIBNAME} PUBLIC pthread ${_REFLECTION} ${_GRPC_GRPCPP} ${_PROTOBUF_LIBPROTOBUF})
 
# set_target_properties(${LIBNAME} PROPERTIES FOLDER "io"
#     APPEND_STRING PROPERTY LINK_FLAGS " ${ADD_LINK_LIB_FLAGS}")

option(USE_ALONE "" OFF)
IF(USE_ALONE)
    add_definitions(-DUSE_ALONE=1)
ENDIF()

option(ASYNC_SERVER "" OFF)
IF(ASYNC_SERVER)
    add_definitions(-DASYNC_SERVER=1)
ENDIF()

# targets pybind11
add_subdirectory("./third_party/pybind11/" pybind11)
pybind11_add_module(io_channel core/pybind11/io_channel.cc)
target_link_libraries(io_channel PRIVATE ${LIBNAME})
