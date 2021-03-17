add_executable(test_driver_ds1307 main.c)
target_link_libraries(test_driver_ds1307 PUBLIC RIOT::RIOT)
riot_target_depends(test_driver_ds1307 PUBLIC ds1307 embunit xtimer)
riot_check_missing_modules(test_driver_ds1307)
