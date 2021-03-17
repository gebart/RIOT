add_executable(test_driver_srf08 main.c)
target_link_libraries(test_driver_srf08 PUBLIC RIOT::RIOT)
riot_target_depends(test_driver_srf08 PUBLIC xtimer srf08)
riot_check_missing_modules(test_driver_srf08)
