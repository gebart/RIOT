add_library(riot_cortexm_common STATIC
  cortexm_init.c
  mpu.c
  panic.c
  thread_arch.c
  vectors_cortexm.c
  )
target_include_directories(riot_cortexm_common PUBLIC include)
add_library(RIOT::cortexm_common ALIAS riot_cortexm_common)
