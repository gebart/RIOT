add_executable(test_backtrace main.c)
target_link_libraries(test_backtrace PUBLIC RIOT::RIOT)
riot_target_depends(test_backtrace PUBLIC backtrace)
riot_check_missing_modules(test_backtrace)
