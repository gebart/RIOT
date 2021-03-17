add_executable(test_driver_w5100 main.c)
target_link_libraries(test_driver_w5100 PUBLIC RIOT::RIOT)
riot_target_depends(test_driver_w5100 PUBLIC w5100)
riot_check_missing_modules(test_driver_w5100)
