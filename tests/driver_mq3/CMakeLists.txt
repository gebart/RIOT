add_executable(test_driver_mq3 main.c)
target_link_libraries(test_driver_mq3 PUBLIC RIOT::RIOT)
riot_target_depends(test_driver_mq3 PUBLIC mq3 xtimer)
riot_check_missing_modules(test_driver_mq3)
