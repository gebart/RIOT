add_riot_module(xtimer STATIC xtimer.c xtimer_core.c)
set (RIOT_XTIMER_BACKEND "periph_timer" CACHE STRING "Select xtimer backend")
set_property(CACHE RIOT_XTIMER_BACKEND PROPERTY STRINGS
  "periph_timer"
  "ztimer_xtimer_compat"
  "xtimer_on_ztimer"
)
riot_module_depends(xtimer ${RIOT_XTIMER_BACKEND})
if (NOT RIOT_XTIMER_BACKEND STREQUAL ztimer_xtimer_compat)
  riot_module_depends(xtimer div)
  riot_module_depends(auto_init auto_init_xtimer)
endif()
