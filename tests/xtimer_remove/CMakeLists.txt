add_executable(test_xtimer_remove main.c)
target_link_libraries(test_xtimer_remove PUBLIC RIOT::RIOT)
riot_target_depends(test_xtimer_remove PUBLIC xtimer)
