cmake_minimum_required(VERSION 2.8)
project(final)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR})
include(ExternalProject)

if("${CMAKE_SYSTEM_NAME}" STREQUAL "FreeBSD")
    set(CMD_TO_BUILD gmake)
else()
    set(CMD_TO_BUILD make)
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
    CONFIGURE_COMMAND "./autogen.sh"
    BUILD_IN_SOURCE 1
    BUILD_COMMAND make
    INSTALL_COMMAND ""
)

ExternalProject_Add_Step(
    libuv c1
    COMMAND "./configure"
    DEPENDERS build  
    WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}/libuv
)

include_directories(${PROJECT_SOURCE_DIR}/http-parser ${PROJECT_SOURCE_DIR}/libuv/include)
link_directories(${PROJECT_SOURCE_DIR}/http-parser ${PROJECT_SOURCE_DIR}/libuv)
add_executable(final http_server.cpp)
add_dependencies(final http-parser libuv)
set_property(TARGET final PROPERTY CXX_STANDARD 11)
target_link_libraries(final uv http_parser)
