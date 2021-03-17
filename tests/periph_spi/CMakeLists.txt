add_executable(test_periph_spi main.c)
target_link_libraries(test_periph_spi PUBLIC RIOT::RIOT)
riot_target_depends(test_periph_spi PUBLIC xtimer shell shell_commands schedstatistics)
riot_check_missing_modules(test_periph_spi)
