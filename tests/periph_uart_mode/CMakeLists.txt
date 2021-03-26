add_executable(test_periph_uart_mode main.c)
target_link_libraries(test_periph_uart_mode PUBLIC RIOT::RIOT)
riot_target_depends(test_periph_uart_mode PUBLIC periph_uart periph_uart_modecfg xtimer)

# Set this to prevent welcome message from printing and confusing output
target_compile_definitions(test_periph_uart_mode PUBLIC "LOG_LEVEL=LOG_NONE")

riot_check_missing_modules(test_periph_uart_mode)
