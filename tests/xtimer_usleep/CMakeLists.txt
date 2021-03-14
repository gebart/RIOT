add_executable(test_xtimer_usleep main.c)
target_link_libraries(test_xtimer_usleep PUBLIC RIOT::RIOT)
riot_target_depends(test_xtimer_usleep PUBLIC xtimer)

# Port and pin configuration for probing with oscilloscope
# Port number should be found in port enum e.g in cpu/include/periph_cpu.h
#riot_target_depends(test_xtimer_usleep PUBLIC periph_gpio)
#target_compile_definitions(test_xtimer_usleep PUBLIC SLEEP_PIN=7 SLEEP_PORT=PORT_F)
riot_check_missing_modules(test_xtimer_usleep)
