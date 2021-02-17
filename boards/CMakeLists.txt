if(NOT BOARD)
  message(FATAL_ERROR "BOARD is not defined")
endif()
add_subdirectory(${BOARD})
