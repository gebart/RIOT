add_executable(test_periph_pwm main.c)
target_link_libraries(test_periph_pwm PUBLIC RIOT::RIOT)
riot_target_depends(test_periph_pwm PUBLIC xtimer shell)
riot_check_missing_modules(test_periph_pwm)
