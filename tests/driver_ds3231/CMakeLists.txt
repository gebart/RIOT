add_executable(test_driver_ds3231 main.c)
target_link_libraries(test_driver_ds3231 PUBLIC RIOT::RIOT)
riot_target_depends(test_driver_ds3231 PUBLIC ds3231 xtimer shell)
riot_check_missing_modules(test_driver_ds3231)
