add_executable(test_minimal main.c)
target_link_libraries(test_minimal PUBLIC RIOT::RIOT)
riot_target_depends(test_minimal PUBLIC stdio_null)
riot_check_missing_modules(test_minimal)
