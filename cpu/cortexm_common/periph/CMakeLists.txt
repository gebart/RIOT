add_riot_module(cortexm_common_periph STATIC
  pm.c
  )
riot_use_module(cortexm_common_periph)
riot_module_depends(cortexm_common_periph cortexm_common)
