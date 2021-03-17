add_executable(test_memarray main.c)
target_link_libraries(test_memarray PUBLIC RIOT::RIOT)
riot_target_depends(test_memarray PUBLIC memarray shell_commands ps)
riot_check_missing_modules(test_memarray)
