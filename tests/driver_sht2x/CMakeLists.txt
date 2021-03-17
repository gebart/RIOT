add_executable(test_driver_sht2x main.c)
target_link_libraries(test_driver_sht2x PUBLIC RIOT::RIOT)
riot_target_depends(test_driver_sht2x PUBLIC sht2x xtimer)
riot_check_missing_modules(test_driver_sht2x)
