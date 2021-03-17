add_executable(test_periph_timer main.c)
target_link_libraries(test_periph_timer PUBLIC RIOT::RIOT)
riot_target_depends(test_periph_timer PUBLIC )
riot_check_missing_modules(test_periph_timer)
