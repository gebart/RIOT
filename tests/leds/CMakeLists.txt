add_executable(test_leds main.c)
target_link_libraries(test_leds PUBLIC RIOT::RIOT)
riot_target_depends(test_leds PUBLIC )
riot_check_missing_modules(test_leds)
