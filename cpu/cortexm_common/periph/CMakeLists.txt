add_riot_module(cortexm_common_periph RIOT::cortexm_common_periph STATIC
  pm.c
  )
riot_use_module(RIOT::cortexm_common_periph)
target_link_libraries(riot_cortexm_common_periph PUBLIC RIOT::cortexm_common)
