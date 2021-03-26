add_executable(test_periph_rtc main.c)
target_link_libraries(test_periph_rtc PUBLIC RIOT::RIOT)
# TODO DISABLE_MODULE periph_init_rtc
riot_target_depends(test_periph_rtc PUBLIC periph_rtc xtimer)
riot_check_missing_modules(test_periph_rtc)
