
set(LINKER_SCRIPT "kinetis.ld")
add_library(riot_cpu_flags INTERFACE)
target_link_libraries(riot_flags INTERFACE riot_cpu_flags)
target_compile_options(riot_cpu_flags INTERFACE -mcpu=cortex-m4)
target_link_options(riot_cpu_flags INTERFACE -mcpu=cortex-m4 -T "${LINKER_SCRIPT}" "-L${CMAKE_CURRENT_SOURCE_DIR}/ldscripts")
add_library(riot_cpu OBJECT
  cpu.c
  fcfield.c
  isr_kinetis.c
  vectors.c
  )
add_library(RIOT::cpu ALIAS riot_cpu)
add_library(riot_cpu_config INTERFACE)
add_library(RIOT::cpu::config ALIAS riot_cpu_config)
target_include_directories(riot_cpu_config INTERFACE include)
get_filename_component(PARENT_DIR ${CMAKE_CURRENT_SOURCE_DIR} DIRECTORY)
target_link_libraries(riot_cpu PUBLIC
  RIOT::cpu::config
  RIOT::flags
  RIOT::board::config
  RIOT::cortexm_common
)
target_sources(RIOT INTERFACE $<TARGET_OBJECTS:RIOT::cpu>)
set(ROM_START_ADDR 0x00000000)
set(RAM_BASE_ADDR  0x20000000)
math(EXPR RAM_START_ADDR "${RAM_BASE_ADDR} - (${KINETIS_SRAM_L_SIZE} * 1024)" OUTPUT_FORMAT HEXADECIMAL)

# The `K` is correctly handled by both the linker and `cortexm_common`.
set(ROM_LEN "${KINETIS_ROMSIZE}K")
math(EXPR RAM_LEN "${KINETIS_RAMSIZE} * 1024")

target_compile_definitions(riot_cpu_config INTERFACE
  "KINETIS_CORE_${KINETIS_CORE}"
  "KINETIS_SERIES_${KINETIS_SERIES}"
  "KINETIS_FAMILY=${KINETIS_FAMILY}"
  "KINETIS_SUBFAMILY=${KINETIS_SUBFAMILY}"
  "KINETIS_ROMSIZE=${KINETIS_ROMSIZE}")

add_library(riot_cpu_kinetis_clock INTERFACE)
if (KINETIS_FAMILY STREQUAL "ea")
  riot_use_module(RIOT::periph::ics)
elseif (KINETIS_FAMILY STREQUAL "l")
  riot_use_module(RIOT::periph::mcg_lite)
else()
  riot_use_module(RIOT::periph::mcg)
endif()
target_link_libraries(riot_cpu_config INTERFACE riot_cpu_kinetis_clock)

add_subdirectory(${PARENT_DIR}/cortexm_common cortexm_common)
add_subdirectory(periph)
# CPU init requires watchdog disable functionality
riot_use_module(RIOT::periph::wdog)
