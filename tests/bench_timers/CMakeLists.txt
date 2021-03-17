add_executable(test_bench_timers main.c print_results.c spin_random.c)
target_link_libraries(test_bench_timers PUBLIC RIOT::RIOT)
riot_target_depends(test_bench_timers PUBLIC random fmt matstat xtimer)
riot_check_missing_modules(test_bench_timers)
