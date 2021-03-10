add_riot_module(cortexm_common
    cortexm_init.c
    mpu.c
    panic.c
    thread_arch.c
    vectors_cortexm.c
)
riot_module_depends(cpu cortexm_common)
target_link_directories(riot_module_cortexm_common INTERFACE "ldscripts")
file(GLOB ldscript_files LIST_DIRECTORIES false "ldscripts/*")
set_target_properties(riot_module_cortexm_common PROPERTIES INTERFACE_LINK_DEPENDS "${ldscript_files}")
target_include_directories(riot_module_cortexm_common INTERFACE "include" "include/vendor")
set(cortexm_cpu_flags
    "-mcpu=${CPU_CORE}"
    "-mthumb"
    "$<$<COMPILE_LANG_AND_ID:C,GNU>:-mno-thumb-interwork>"
    "$<$<COMPILE_LANG_AND_ID:CXX,GNU>:-mno-thumb-interwork>"
)
if (CPU_ARCH STREQUAL "armv6m")
  # work around https://gcc.gnu.org/bugzilla/show_bug.cgi?id=85606
  list(APPEND cortexm_cpu_flags "-march=armv6s-m")
else()
  list(APPEND cortexm_cpu_flags "-march=${CPU_ARCH}")
endif()

target_compile_options(riot_module_cortexm_common INTERFACE ${cortexm_cpu_flags})
target_link_options(riot_module_cortexm_common INTERFACE ${cortexm_cpu_flags})

target_link_options(riot_module_cortexm_common INTERFACE -nostartfiles)

# All variables must be defined in the CPU configuration when using the common
# `ldscripts/cortexm.ld`
if (NOT "${ROM_START_ADDR}${RAM_START_ADDR}${ROM_LEN}${RAM_LEN}" STREQUAL "")
  message (DEBUG "ROM_START_ADDR: ${ROM_START_ADDR}")
  message (DEBUG "RAM_START_ADDR: ${RAM_START_ADDR}")
  if (NOT ROM_START_ADDR)
    message(FATAL_ERROR "ROM_START_ADDR is not defined")
  endif()
  if (NOT RAM_START_ADDR)
    message(FATAL_ERROR "RAM_START_ADDR is not defined")
  endif()
  if (NOT ROM_LEN)
    message(FATAL_ERROR "ROM_LEN is not defined")
  endif()
  if (NOT RAM_LEN)
    message(FATAL_ERROR "RAM_LEN is not defined")
  endif()
  target_link_options(riot_module_cortexm_common INTERFACE
    "LINKER:--defsym=_rom_start_addr=${ROM_START_ADDR}"
    "LINKER:--defsym=_ram_start_addr=${RAM_START_ADDR}"
    "LINKER:--defsym=_rom_length=${ROM_LEN}"
    "LINKER:--defsym=_ram_length=${RAM_LEN}"
  )
endif()

if (BACKUP_RAM_LEN)
  target_link_options(riot_module_cortexm_common INTERFACE "LINKER:--defsym=_backup_ram_start_addr=${BACKUP_RAM_ADDR}")
  target_link_options(riot_module_cortexm_common INTERFACE "LINKER:--defsym=_backup_ram_len=${BACKUP_RAM_LEN}")
  target_compile_definitions(riot_module_cortexm_common INTERFACE CPU_HAS_BACKUP_RAM=1)
endif()

add_subdirectory(periph)
