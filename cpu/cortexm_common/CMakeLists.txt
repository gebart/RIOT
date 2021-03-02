add_riot_module(cortexm_common OBJECT
  cortexm_init.c
  mpu.c
  panic.c
  thread_arch.c
  vectors_cortexm.c
  )
riot_use_module(cortexm_common)
target_link_libraries(riot_module_cortexm_common PUBLIC RIOT::core RIOT::board::config RIOT::cpu::config)
target_link_options(riot_module_cortexm_common PUBLIC -L "${CMAKE_CURRENT_SOURCE_DIR}/ldscripts")
target_include_directories(riot_module_cortexm_common PUBLIC include include/vendor)
target_include_directories(riot_cpu_config INTERFACE include include/vendor)
set(cortexm_cpu_flags 
  -mcpu=${CPU_CORE}
  -mthumb
)
if (CPU_ARCH STREQUAL "armv6m")
  # work around https://gcc.gnu.org/bugzilla/show_bug.cgi?id=85606
  list(APPEND cortexm_cpu_flags -march=armv6s-m)
else()
  list(APPEND cortexm_cpu_flags -march=${CPU_ARCH})
endif()

target_compile_options(riot_cpu_flags INTERFACE
  ${cortexm_cpu_flags}
  $<$<COMPILE_LANG_AND_ID:C,GNU>:-mno-thumb-interwork>
  $<$<COMPILE_LANG_AND_ID:CXX,GNU>:-mno-thumb-interwork>
)
target_link_options(riot_cpu_flags INTERFACE
  ${cortexm_cpu_flags}
  $<$<LINK_LANG_AND_ID:C,GNU>:-mno-thumb-interwork>
  $<$<LINK_LANG_AND_ID:CXX,GNU>:-mno-thumb-interwork>
)

target_link_options(riot_module_cortexm_common PUBLIC -nostartfiles)

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
  target_link_options(riot_module_cortexm_common PUBLIC
    LINKER:--defsym=_rom_start_addr=${ROM_START_ADDR}
    LINKER:--defsym=_ram_start_addr=${RAM_START_ADDR}
    LINKER:--defsym=_rom_length=${ROM_LEN}
    LINKER:--defsym=_ram_length=${RAM_LEN}
  )
endif()
add_subdirectory(periph)
