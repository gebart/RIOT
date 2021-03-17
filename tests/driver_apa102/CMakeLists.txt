add_executable(test_driver_apa102 main.c)
target_link_libraries(test_driver_apa102 PUBLIC RIOT::RIOT)
riot_target_depends(test_driver_apa102 PUBLIC apa102 color xtimer)
riot_check_missing_modules(test_driver_apa102)
