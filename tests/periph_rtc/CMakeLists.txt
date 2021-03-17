add_executable(test_periph_rtc main.c)
target_link_libraries(test_periph_rtc PUBLIC RIOT::RIOT)
riot_target_depends(test_periph_rtc PUBLIC xtimer)
riot_check_missing_modules(test_periph_rtc)
