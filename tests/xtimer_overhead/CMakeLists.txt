add_executable(test_xtimer_overhead main.c)
target_link_libraries(test_xtimer_overhead PUBLIC RIOT::RIOT)
riot_target_depends(test_xtimer_overhead PUBLIC xtimer)
