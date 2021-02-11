if(NOT CPU)
  message(FATAL_ERROR "CPU is not defined")
endif()
set(RIOT_CPU_DIR ${CMAKE_CURRENT_SOURCE_DIR}/${CPU})

add_subdirectory(common)
add_subdirectory(${RIOT_CPU_DIR})
