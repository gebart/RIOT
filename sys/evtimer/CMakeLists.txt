add_riot_module(evtimer evtimer.c)
add_riot_module(evtimer_on_ztimer)
riot_module_depends(evtimer_on_ztimer ztimer_msec)
# TODO conditional else case: evtimer depends on xtimer
