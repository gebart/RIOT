
set(LINKER_SCRIPT "kinetis.ld")
add_library(riot_cpu_flags INTERFACE)
target_link_libraries(riot_flags INTERFACE riot_cpu_flags)
target_compile_options(riot_cpu_flags INTERFACE -mcpu=cortex-m4)
target_link_options(riot_cpu_flags INTERFACE -mcpu=cortex-m4 "-T ${LINKER_SCRIPT}" "-L${CURRENT_SOURCE_DIR}/ldscripts")
add_library(riot_cpu STATIC
  cpu.c
  )
get_filename_component(PARENT_DIR ${CMAKE_CURRENT_SOURCE_DIR} DIRECTORY)
add_subdirectory(${PARENT_DIR}/cortexm_common cortexm_common)
target_link_libraries(riot_cpu PUBLIC RIOT::cortexm_common)
target_include_directories(riot_cpu PUBLIC include)

target_compile_definitions(riot_cpu PUBLIC 
  KINETIS_CORE_$(KINETIS_CORE)
  KINETIS_SERIES_$(KINETIS_SERIES)
  KINETIS_FAMILY=$(KINETIS_FAMILY)
  KINETIS_SUBFAMILY=$(KINETIS_SUBFAMILY)
  KINETIS_ROMSIZE=$(KINETIS_ROMSIZE))
