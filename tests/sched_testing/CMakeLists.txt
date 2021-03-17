add_executable(test_sched_testing main.c)
target_link_libraries(test_sched_testing PUBLIC RIOT::RIOT)
riot_target_depends(test_sched_testing PUBLIC )
riot_check_missing_modules(test_sched_testing)
