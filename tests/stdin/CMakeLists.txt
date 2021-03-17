add_executable(test_stdin main.c)
target_link_libraries(test_stdin PUBLIC RIOT::RIOT)
riot_target_depends(test_stdin PUBLIC stdin)
riot_check_missing_modules(test_stdin)
