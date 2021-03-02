cmake_minimum_required(VERSION 3.18)
project(test_thread_cooperation)

set(RIOT_BASEDIR ${CMAKE_CURRENT_LIST_DIR}/../../)
list(APPEND CMAKE_MODULE_PATH "${RIOT_BASEDIR}/cmake" "${CMAKE_MODULE_PATH}")
include(RIOT)
add_executable(thread_cooperation main.c)
riot_configure_target(thread_cooperation)

# some boards have too little memory for this test
set(small_boards nucleo-f030r8 nucleo-l031k6 nucleo-l053r8 stm32f0discovery stm32l0538-disco)
if (BOARD STREQUAL "nucleo-f042k6")
  set(PROBLEM 3)
elseif (BOARD IN_LIST small_boards)
  set(PROBLEM 5)
elseif (BOARD STREQUAL "nucleo-f303k8")
  set(PROBLEM 7)
elseif (BOARD STREQUAL "nucleo-f334r8")
  set(PROBLEM 9)
endif()
if (PROBLEM)
  target_compile_definitions(thread_cooperation PRIVATE "PROBLEM=${PROBLEM}")
endif()

