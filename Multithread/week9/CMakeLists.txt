cmake_minimum_required(VERSION 2.8)
project(final)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR})
include(ExternalProject)

if("${CMAKE_SYSTEM_NAME}" STREQUAL "FreeBSD")
    set(CMD_TO_BUILD gmake)
    set(SYSTEM_LIB kvm)
else()
    set(CMD_TO_BUILD make)
    set(SYSTEM_LIB "")
endif()

ExternalProject_Add(
    http-parser
    GIT_REPOSITORY "https://github.com/nodejs/http-parser.git"
    SOURCE_DIR ${PROJECT_SOURCE_DIR}/http-parser
    CONFIGURE_COMMAND ""
    BUILD_IN_SOURCE 1
    BUILD_COMMAND ${CMD_TO_BUILD} package
    INSTALL_COMMAND ""
)

ExternalProject_Add(
    libuv
    GIT_REPOSITORY "https://github.com/libuv/libuv.git"
    SOURCE_DIR ${PROJECT_SOURCE_DIR}/libuv
    CONFIGURE_COMMAND ./gyp_uv.py -f make
    BUILD_IN_SOURCE 1
    BUILD_COMMAND BUILDTYPE=Release ${CMD_TO_BUILD} -C out
    INSTALL_COMMAND ""
)

ExternalProject_Add_Step(
    libuv c1
    COMMAND git clone https://chromium.googlesource.com/external/gyp.git build/gyp
    DEPENDEES update
    DEPENDERS configure
    WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}/libuv
)

#ExternalProject_Add_Step(
#    libuv c1
#    COMMAND "./configure"
#    DEPENDEES configure
#    DEPENDERS build  
#    WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}/libuv
#)

find_package(Threads REQUIRED)
add_definitions(-std=c++11)
include_directories(${PROJECT_SOURCE_DIR}/http-parser ${PROJECT_SOURCE_DIR}/libuv/include)
link_directories(${PROJECT_SOURCE_DIR}/http-parser ${PROJECT_SOURCE_DIR}/libuv/out/Release)
add_executable(final http_server.cpp)
add_dependencies(final http-parser libuv)
set_property(TARGET final PROPERTY CXX_STANDARD 11)
set_property(TARGET final PROPERTY CXX_STANDARD_REQUIRED ON)
target_link_libraries(final uv ${SYSTEM_LIB} http_parser ${CMAKE_THREAD_LIBS_INIT})
