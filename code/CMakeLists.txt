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

project(io_channel C CXX)

include(../cmake/common.cmake)
add_definitions(-std=c++11)
# Proto file
get_filename_component(io_proto "../protos/io_channel.proto" ABSOLUTE)
get_filename_component(io_proto_path "${io_proto}" PATH)

# Generated sources
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
SET(THIRD_PARTY_DIR "../third_party")
include_directories(${THIRD_PARTY_DIR}/rapidjson/include)

# add io files
set(config_h "${CMAKE_CURRENT_BINARY_DIR}/include/config.h")
set(config_cc "${CMAKE_CURRENT_BINARY_DIR}/src/config.cc")
set(io_channel_h "${CMAKE_CURRENT_BINARY_DIR}/include/io_channel.h")
set(conn_h "${CMAKE_CURRENT_BINARY_DIR}/include/connection.h")
set(conn_cc "${CMAKE_CURRENT_BINARY_DIR}/src/connection.cc")
set(netio_h "${CMAKE_CURRENT_BINARY_DIR}/include/net_io.h")
set(netio_cc "${CMAKE_CURRENT_BINARY_DIR}/src/net_io.cc")
set(impl_h "${CMAKE_CURRENT_BINARY_DIR}/include/io_channel_impl.h")
set(impl_cc "${CMAKE_CURRENT_BINARY_DIR}/src/io_channel_impl.cc")

# hw_grpc_proto
add_library(hw_grpc_proto
  ${config_cc}  ${config_h}  ${conn_h} ${conn_cc} ${io_channel_h} ${netio_cc} ${netio_h} 
  ${impl_cc} ${impl_h}
  ${io_grpc_srcs} ${io_grpc_hdrs} ${io_proto_srcs} ${io_proto_hdrs})

target_link_libraries(hw_grpc_proto
  ${_REFLECTION}
  ${_GRPC_GRPCPP}
  ${_PROTOBUF_LIBPROTOBUF}
  -lstdc++)

# Targets greeter_[async_](client|server)
foreach(_target
  #test_client server)
  #server client)
  #p0_server p1_client)
  #p0_server p1_client p2_client p9_client)
  p0_cs p1_cs p2_cs p9_cs)
  add_executable(${_target} "${_target}.cc")
  target_link_libraries(${_target}
    hw_grpc_proto
    ${_REFLECTION}
    ${_GRPC_GRPCPP}
    ${_PROTOBUF_LIBPROTOBUF})
endforeach()
