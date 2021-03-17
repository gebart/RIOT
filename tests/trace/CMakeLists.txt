add_executable(test_trace main.c)
target_link_libraries(test_trace PUBLIC RIOT::RIOT)
riot_target_depends(test_trace PUBLIC trace)
riot_check_missing_modules(test_trace)
