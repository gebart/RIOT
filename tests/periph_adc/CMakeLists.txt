add_executable(test_periph_adc main.c)
target_link_libraries(test_periph_adc PUBLIC RIOT::RIOT)
riot_target_depends(test_periph_adc PUBLIC periph_adc xtimer)
