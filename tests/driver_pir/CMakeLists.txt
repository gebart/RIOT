add_executable(test_driver_pir main.c)
target_link_libraries(test_driver_pir PUBLIC RIOT::RIOT)
riot_target_depends(test_driver_pir PUBLIC pir)
riot_check_missing_modules(test_driver_pir)
