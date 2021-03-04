cmake_minimum_required(VERSION 3.18)
project(timer_periodic_wakeup)

set(RIOT_BASEDIR ${CMAKE_CURRENT_LIST_DIR}/../../)
list(APPEND CMAKE_MODULE_PATH "${RIOT_BASEDIR}/cmake" "${CMAKE_MODULE_PATH}")
include(RIOT)

# This example uses xtimer for scheduling wakeups
riot_use_module(xtimer)

add_executable(${PROJECT_NAME} main.c)
riot_configure_target(${PROJECT_NAME})
