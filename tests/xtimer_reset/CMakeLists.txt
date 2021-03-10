add_executable(test_xtimer_reset main.c)
target_link_libraries(test_xtimer_reset PUBLIC RIOT::RIOT)
riot_target_depends(test_xtimer_reset PUBLIC xtimer)
