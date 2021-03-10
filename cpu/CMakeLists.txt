if(NOT RIOT_CPU_DIR)
  message(FATAL_ERROR "RIOT_CPU_DIR is not defined")
endif()

add_subdirectory(${RIOT_CPU_DIR})

# These modules should be defined in every cpu and board CMakeLists.txt
riot_module_depends(cpu board)
target_compile_definitions(riot_module_cpu INTERFACE "CPU_MODEL_${CPU_MODEL}")
riot_module_depends(core cpu)
