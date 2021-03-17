add_executable(test_periph_uart main.c)
target_link_libraries(test_periph_uart PUBLIC RIOT::RIOT)
riot_target_depends(test_periph_uart PUBLIC shell xtimer)
riot_check_missing_modules(test_periph_uart)
