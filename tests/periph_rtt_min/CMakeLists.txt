add_executable(test_periph_rtt_min main.c)
target_link_libraries(test_periph_rtt_min PUBLIC RIOT::RIOT)
# TODO DISABLE_MODULE periph_init_rtt
riot_target_depends(test_periph_rtt_min PUBLIC periph_rtt xtimer)
riot_check_missing_modules(test_periph_rtt_min)
