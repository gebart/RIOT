add_executable(test_trickle main.c)
target_link_libraries(test_trickle PUBLIC RIOT::RIOT)
riot_target_depends(test_trickle PUBLIC trickle)
riot_check_missing_modules(test_trickle)
