cmake_minimum_required(VERSION 3.18)
project(hello_world)

set(RIOT_BASEDIR ${CMAKE_CURRENT_LIST_DIR}/../../)
list(APPEND CMAKE_MODULE_PATH "${RIOT_BASEDIR}/cmake" "${CMAKE_MODULE_PATH}")
include(RIOT)
add_executable(hello_world main.c)
riot_configure_target(hello_world)
