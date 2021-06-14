# Copyright 2021 io_channel authors.
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
#
# cmake build file for C++ io_channel example.
# Assumes protobuf and gRPC have been installed using cmake.
# See cmake_externalproject/CMakeLists.txt for all-in-one cmake build
# that automatically builds all the dependencies before building io_channel.

cmake_minimum_required(VERSION 3.5.1)

project(channel_sdk C CXX)

include(./cmake/common.cmake)
add_definitions(-std=c++11)

#file(MAKE_DIRECTORY ${lib_path})
set(LIBRARY_OUTPUT_PATH ${CMAKE_CURRENT_SOURCE_DIR}/lib)
# Proto file
get_filename_component(io_proto "./protos/io_channel.proto" ABSOLUTE)
get_filename_component(io_proto_path "${io_proto}" PATH)

# Generated sources
file(MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/protos)  
set(out_proto_dirs "${CMAKE_CURRENT_BINARY_DIR}/protos")
set(io_proto_srcs "${out_proto_dirs}/io_channel.pb.cc")
set(io_proto_hdrs "${out_proto_dirs}/io_channel.pb.h")
set(io_grpc_srcs "${out_proto_dirs}/io_channel.grpc.pb.cc")
set(io_grpc_hdrs "${out_proto_dirs}/io_channel.grpc.pb.h")
add_custom_command(
      OUTPUT "${io_proto_srcs}" "${io_proto_hdrs}" "${io_grpc_srcs}" "${io_grpc_hdrs}"
      COMMAND ${_PROTOBUF_PROTOC}
      ARGS --grpc_out "${out_proto_dirs}"
        --cpp_out "${out_proto_dirs}"
        -I "${io_proto_path}"
        --plugin=protoc-gen-grpc="${_GRPC_CPP_PLUGIN_EXECUTABLE}"
        "${io_proto}"
      DEPENDS "${io_proto}")

# Include generated *.pb.h files
include_directories("${CMAKE_CURRENT_BINARY_DIR}")
# third party
SET(THIRD_PARTY_DIR "./third_party")
include_directories(${THIRD_PARTY_DIR}/rapidjson/include)

# add core path
SET(core_path "${CMAKE_CURRENT_SOURCE_DIR}/core")
set(config_h "${core_path}/include/config.h")
set(config_cc "${core_path}/src/config.cc")
set(io_channel_h "${core_path}/include/IChannel.h")
set(conn_h "${core_path}/include/connection.h")
set(conn_cc "${core_path}/src/connection.cc")
set(netio_h "${core_path}/include/net_io.h")
set(netio_cc "${core_path}/src/net_io.cc")
set(impl_h "${core_path}/include/io_channel_impl.h")
set(impl_cc "${core_path}/src/io_channel_impl.cc")

# io_grpc_proto
add_library(io_grpc_proto
  ${config_cc}  ${config_h} ${conn_h} ${conn_cc} ${io_channel_h} ${netio_cc} ${netio_h} 
  ${impl_cc} ${impl_h} 
  ${io_grpc_srcs} ${io_grpc_hdrs} ${io_proto_srcs} ${io_proto_hdrs})

target_link_libraries(io_grpc_proto
  ${_REFLECTION}
  ${_GRPC_GRPCPP}
  ${_PROTOBUF_LIBPROTOBUF})

include_directories("${core_path}")
# Targets demo(client|server)
# foreach(_target
#   #client)
#   #test_client server)
#   #server client)
#   #p0_server p1_client)
#   # p0_server p1_client p2_client)
#   # io_channel_async_server io_channel_async_client io_channel_async_client2 
#   p0_server p1_client p2_client
#   #p0_cs p1_cs p2_cs p9_cs
#   )
#   add_executable(${_target} "${_target}.cc")
#   target_link_libraries(${_target}
#     io_grpc_proto
#     ${_REFLECTION}
#     ${_GRPC_GRPCPP}
#     ${_PROTOBUF_LIBPROTOBUF})
# endforeach()

# targets pybind11
add_subdirectory("./third_party/pybind11/" pybind11)
set(io_include_files
  ${config_cc}  ${config_h} ${conn_h} ${conn_cc} ${io_channel_h} ${netio_cc} ${netio_h} 
  ${impl_cc} ${impl_h}
  ${io_grpc_srcs}
  ${io_grpc_hdrs}
  ${io_proto_srcs}
  ${io_proto_hdrs})

pybind11_add_module(io_channel ${core_path}/src/io_channel.cc ${io_include_files})
target_link_libraries(io_channel PRIVATE
	${_REFLECTION}
	${_GRPC_GRPCPP}
	${_PROTOBUF_LIBPROTOBUF})
	