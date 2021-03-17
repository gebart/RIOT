add_executable(test_driver_xbee main.c)
target_link_libraries(test_driver_xbee PUBLIC RIOT::RIOT)
riot_target_depends(test_driver_xbee PUBLIC xbee)
riot_check_missing_modules(test_driver_xbee)
