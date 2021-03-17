add_executable(test_frac-config main.c)
target_link_libraries(test_frac-config PUBLIC RIOT::RIOT)
riot_target_depends(test_frac-config PUBLIC frac)
riot_check_missing_modules(test_frac-config)
