add_riot_module(evtimer_impl evtimer.c)
add_riot_module(evtimer_on_ztimer)
add_riot_module(evtimer_on_xtimer)
riot_module_depends(evtimer_on_ztimer evtimer_impl ztimer_msec)
riot_module_depends(evtimer_on_xtimer evtimer_impl xtimer)
# TODO conditional else case: evtimer depends on xtimer
add_riot_module(evtimer)
riot_module_depends(evtimer evtimer_on_xtimer)
