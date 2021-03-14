add_executable(test_xtimer_msg main.c)
target_link_libraries(test_xtimer_msg PUBLIC RIOT::RIOT)
riot_target_depends(test_xtimer_msg PUBLIC xtimer)
riot_check_missing_modules(test_xtimer_msg)
