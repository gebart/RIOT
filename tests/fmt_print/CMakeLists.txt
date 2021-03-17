add_executable(test_fmt_print main.c)
target_link_libraries(test_fmt_print PUBLIC RIOT::RIOT)
riot_target_depends(test_fmt_print PUBLIC fmt)
riot_check_missing_modules(test_fmt_print)
