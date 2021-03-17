add_executable(test_driver_sht3x main.c)
target_link_libraries(test_driver_sht3x PUBLIC RIOT::RIOT)
riot_target_depends(test_driver_sht3x PUBLIC sht3x)
riot_check_missing_modules(test_driver_sht3x)
