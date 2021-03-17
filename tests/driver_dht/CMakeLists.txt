add_executable(test_driver_dht main.c)
target_link_libraries(test_driver_dht PUBLIC RIOT::RIOT)
riot_target_depends(test_driver_dht PUBLIC dht fmt xtimer)
riot_check_missing_modules(test_driver_dht)
