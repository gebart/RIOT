add_executable(test_periph_rtt main.c)
target_link_libraries(test_periph_rtt PUBLIC RIOT::RIOT)
riot_target_depends(test_periph_rtt PUBLIC )
riot_check_missing_modules(test_periph_rtt)
