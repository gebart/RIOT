set(LINKER_SCRIPT "kinetis.ld")
add_library(riot_cpu_flags INTERFACE)
target_link_libraries(riot_flags INTERFACE riot_cpu_flags)
target_compile_options(riot_cpu_flags INTERFACE -mcpu=cortex-m4)
target_link_options(riot_cpu_flags INTERFACE -mcpu=cortex-m4 "-T ${LINKER_SCRIPT}" "-L${CURRENT_SOURCE_DIR}/ldscripts")
add_library(riot_cpu STATIC
  cpu.c
  )

target_include_directories(riot_cpu PUBLIC include)
