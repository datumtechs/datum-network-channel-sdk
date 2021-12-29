cmake_minimum_required(VERSION 3.5.1)

SET(LIBNAME extio)
project(${LIBNAME})

set(project_path ${CMAKE_CURRENT_SOURCE_DIR}/../../..)
#include(${project_path}/cmake/SetCommon.cmake)
add_definitions(-std=c++11)

# option
# option(SSL_TYPE "" OFF)
set(SSL_TYPE 0)
message(STATUS "[SSL_TYPE]: ${SSL_TYPE}")
add_definitions(-DSSL_TYPE=${SSL_TYPE})

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
include_directories(${project_path}/third_party/rapidjson/include)

# threads
find_package(Threads REQUIRED)

SET(ADD_LINK_LIB_FLAGS "-Wl,--rpath=$ORIGIN../:$ORIGIN")
SET(CMAKE_EXE_LINKER_FLAGS  "${CMAKE_EXE_LINKER_FLAGS} ${ADD_LINK_LIB_FLAGS}")

# IF(SSL_TYPE MATCHES 2)
#     # gmssl
#     set(THIRD_PARTY_DIR ${CMAKE_CURRENT_SOURCE_DIR}/third_party)
#     include_directories(${THIRD_PARTY_DIR}/gmssl/include)
#     link_directories(${THIRD_PARTY_DIR}/gmssl/lib)
#     link_libraries(tacrypto tassl)
# ENDIF()

# libraries
file(GLOB_RECURSE srcs ${project_path}/core/src/*.cc ${project_path}/core/src/*.cpp)
add_library(${LIBNAME} SHARED ${srcs})
target_include_directories(${LIBNAME} PUBLIC ${project_path}/core/include)

# ice
set(THIRD_PARTY_DIR ${project_path}/third_party)
include_directories(${THIRD_PARTY_DIR}/ice/include)
link_directories(${THIRD_PARTY_DIR}/ice/lib)
link_libraries(Ice)

target_link_libraries(${LIBNAME} PUBLIC Ice pthread)

# Use to find the directory dependency path(libtacrypto, libtassl)
set_target_properties(${LIBNAME} PROPERTIES FOLDER ${LIBNAME}
    APPEND_STRING PROPERTY LINK_FLAGS "${ADD_LINK_LIB_FLAGS}")

foreach(_target
p0_server
p1_client
)
add_executable(${_target} ${project_path}/test/c++/one_to_one/${_target}.cc)
target_link_libraries(${_target} PRIVATE ${LIBNAME})
endforeach()
