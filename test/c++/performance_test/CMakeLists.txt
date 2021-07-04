cmake_minimum_required(VERSION 3.5.1)

SET(LIBNAME external_io)
project(${LIBNAME})

set(project_path ${CMAKE_CURRENT_SOURCE_DIR}/../../..)

include(${project_path}/cmake/common.cmake)
add_definitions(-std=c++11)


# third party
include_directories(${project_path}/third_party/rapidjson/include)

# threads
find_package(Threads REQUIRED)

SET(ADD_LINK_LIB_FLAGS "-Wl,--rpath=$ORIGIN../:$ORIGIN")
SET(CMAKE_EXE_LINKER_FLAGS  "${CMAKE_EXE_LINKER_FLAGS} ${ADD_LINK_LIB_FLAGS}")


message(STATUS "_REFLECTION: ${_REFLECTION}")
message(STATUS "_GRPC_GRPCPP: ${_GRPC_GRPCPP}")
message(STATUS "_PROTOBUF_LIBPROTOBUF: ${_PROTOBUF_LIBPROTOBUF}")

link_libraries(${_REFLECTION} ${_GRPC_GRPCPP} ${_PROTOBUF_LIBPROTOBUF})
# libraries
file(GLOB_RECURSE srcs ${project_path}/core/src/*.cc)
add_library(${LIBNAME} SHARED ${srcs})
target_include_directories(${LIBNAME} PUBLIC ${project_path}/core/include)
target_link_libraries(${LIBNAME} PUBLIC pthread ${_REFLECTION} ${_GRPC_GRPCPP} ${_PROTOBUF_LIBPROTOBUF})
 

option(USE_ALONE "" OFF)
IF(USE_ALONE)
    add_definitions(-DUSE_ALONE=1)
ENDIF()

option(ASYNC_SERVER "" OFF)
IF(ASYNC_SERVER)
    add_definitions(-DASYNC_SERVER=1)
ENDIF()

option(ASYNC_CLIENT "" OFF)
IF(ASYNC_CLIENT)
    add_definitions(-DASYNC_CLIENT=1)
ENDIF()

foreach(_target
  p0_cs
  p1_cs
  p2_cs
  )
  add_executable(${_target} ${project_path}/test/c++/performance_test/${_target}.cc)
  target_link_libraries(${_target} PRIVATE ${LIBNAME})
endforeach()




