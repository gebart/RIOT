add_executable(test_driver_hsc main.c)
target_link_libraries(test_driver_hsc PUBLIC RIOT::RIOT)
riot_target_depends(test_driver_hsc PUBLIC hsc xtimer)
riot_check_missing_modules(test_driver_hsc)
