add_riot_module(cpu
    cpu.c
    fcfield.c
    isr_kinetis.c
    vectors.c
)
target_link_options(riot_module_cpu INTERFACE -T "kinetis.ld")
target_link_directories(riot_module_cpu INTERFACE "ldscripts")
set_target_properties(riot_module_cpu PROPERTIES INTERFACE_LINK_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/ldscripts/kinetis.ld")
target_include_directories(riot_module_cpu INTERFACE "include")
riot_module_depends(cpu cortexm_common)

set(ROM_START_ADDR 0x00000000)
set(RAM_BASE_ADDR  0x20000000)
math(EXPR RAM_START_ADDR "${RAM_BASE_ADDR} - (${KINETIS_SRAM_L_SIZE} * 1024)" OUTPUT_FORMAT HEXADECIMAL)

# The `K` is correctly handled by both the linker and `cortexm_common`.
set(ROM_LEN "${KINETIS_ROMSIZE}K")
math(EXPR RAM_LEN "${KINETIS_RAMSIZE} * 1024")

target_compile_definitions(riot_module_cpu INTERFACE
  "KINETIS_CORE_${KINETIS_CORE}"
  "KINETIS_SERIES_${KINETIS_SERIES}"
  "KINETIS_FAMILY=${KINETIS_FAMILY}"
  "KINETIS_SUBFAMILY=${KINETIS_SUBFAMILY}"
  "KINETIS_ROMSIZE=${KINETIS_ROMSIZE}")

if (KINETIS_FAMILY STREQUAL "ea")
  riot_module_depends(cpu periph_ics)
elseif (KINETIS_FAMILY STREQUAL "l")
  riot_module_depends(cpu periph_mcg_lite)
else()
  riot_module_depends(cpu periph_mcg)
endif()

get_filename_component(PARENT_DIR ${CMAKE_CURRENT_SOURCE_DIR} DIRECTORY)
add_subdirectory(${PARENT_DIR}/cortexm_common cortexm_common)
add_subdirectory(periph)
# CPU init requires watchdog disable functionality
riot_module_depends(cpu periph_wdog)
