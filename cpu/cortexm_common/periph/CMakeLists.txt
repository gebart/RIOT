add_riot_module(cortexm_common_periph
    pm.c
)
riot_module_depends(RIOT cortexm_common_periph)
riot_module_depends(cortexm_common_periph cortexm_common)
