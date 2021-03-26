add_executable(test_periph_gpio main.c)
target_link_libraries(test_periph_gpio PUBLIC RIOT::RIOT)
riot_target_depends(test_periph_gpio PUBLIC periph_gpio shell shell_commands benchmark)
riot_target_depends_optional(test_periph_gpio PUBLIC periph_gpio_irq periph_gpio_fast_irq periph_gpio_tamper_wake)

if (BOARD STREQUAL "native")
  riot_target_depends(test_periph_gpio PUBLIC periph_gpio_mock)
endif()
riot_check_missing_modules(test_periph_gpio)
