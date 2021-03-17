add_executable(test_evtimer_msg main.c)
target_link_libraries(test_evtimer_msg PUBLIC RIOT::RIOT)
riot_target_depends(test_evtimer_msg PUBLIC evtimer xtimer)
riot_check_missing_modules(test_evtimer_msg)
