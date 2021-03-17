add_executable(test_driver_sps30 main.c)
target_link_libraries(test_driver_sps30 PUBLIC RIOT::RIOT)
riot_target_depends(test_driver_sps30 PUBLIC sps30 xtimer)
riot_check_missing_modules(test_driver_sps30)
