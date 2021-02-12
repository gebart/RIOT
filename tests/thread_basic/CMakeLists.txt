cmake_minimum_required(VERSION 3.18)
project(test_thread_basic)

set(RIOT_BASEDIR ${CMAKE_CURRENT_LIST_DIR}/../../)
list(APPEND CMAKE_MODULE_PATH "${RIOT_BASEDIR}/cmake" "${CMAKE_MODULE_PATH}")
include(RIOT)
add_executable(thread_basic main.c)
riot_configure_target(thread_basic)
