if(NOT CPU)
  message(FATAL_ERROR "CPU is not defined")
endif()

target_compile_definitions(riot_flags INTERFACE "CPU_MODEL_${CPU_MODEL}")

add_subdirectory(${CPU})
