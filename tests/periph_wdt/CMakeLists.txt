add_executable(test_periph_wdt main.c)
target_link_libraries(test_periph_wdt PUBLIC RIOT::RIOT)
riot_target_depends(test_periph_wdt PUBLIC xtimer shell)
riot_check_missing_modules(test_periph_wdt)
