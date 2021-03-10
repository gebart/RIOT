add_executable(test_xtimer_longterm main.c)
target_link_libraries(test_xtimer_longterm PUBLIC RIOT::RIOT)
riot_target_depends(test_xtimer_longterm PUBLIC xtimer)
