add_executable(test_periph_i2c main.c)
target_link_libraries(test_periph_i2c PUBLIC RIOT::RIOT)
riot_target_depends(test_periph_i2c PUBLIC shell shell_commands i2c_scan xtimer)
riot_check_missing_modules(test_periph_i2c)
