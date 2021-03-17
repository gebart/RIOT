add_executable(test_log_color main.c)
target_link_libraries(test_log_color PUBLIC RIOT::RIOT)
riot_target_depends(test_log_color PUBLIC log_color)
riot_check_missing_modules(test_log_color)
