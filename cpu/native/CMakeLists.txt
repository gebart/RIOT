add_library(riot_cpu_${CPU} STATIC
  async_read.c
  irq_cpu.c
  native_cpu.c
  panic.c
  startup.c
  syscalls.c
  )
add_subdirectory(periph)
target_include_directories(riot_cpu_${CPU} PUBLIC include)
target_link_libraries(riot_cpu_${CPU} PUBLIC riot_core)
