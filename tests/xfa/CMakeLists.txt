add_executable(test_xfa main.c)
target_link_libraries(test_xfa PUBLIC RIOT::RIOT)
riot_target_depends(test_xfa PUBLIC )
riot_check_missing_modules(test_xfa)
