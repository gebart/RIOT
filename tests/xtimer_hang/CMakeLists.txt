add_executable(test_xtimer_hang main.c)
target_link_libraries(test_xtimer_hang PUBLIC RIOT::RIOT)
riot_target_depends(test_xtimer_hang PUBLIC xtimer)
riot_check_missing_modules(test_xtimer_hang)
