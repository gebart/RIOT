cmake_minimum_required(VERSION 3.18)
project(test_periph_cpuid)

set(RIOT_BASEDIR "${CMAKE_CURRENT_LIST_DIR}/../../")
list(APPEND CMAKE_MODULE_PATH "${RIOT_BASEDIR}/cmake" "${CMAKE_MODULE_PATH}")
include(RIOT)
add_executable(${PROJECT_NAME} main.c)
riot_configure_target(${PROJECT_NAME})
riot_use_module(periph_cpuid)
