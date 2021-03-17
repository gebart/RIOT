add_executable(test_irq main.c)
target_link_libraries(test_irq PUBLIC RIOT::RIOT)
riot_target_depends(test_irq PUBLIC xtimer)
riot_check_missing_modules(test_irq)
