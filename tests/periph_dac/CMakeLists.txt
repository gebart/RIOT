add_executable(test_periph_dac main.c)
target_link_libraries(test_periph_dac PUBLIC RIOT::RIOT)
riot_target_depends(test_periph_dac PUBLIC periph_dac xtimer)
riot_check_missing_modules(test_periph_dac)
