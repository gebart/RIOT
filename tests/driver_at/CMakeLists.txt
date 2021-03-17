add_executable(test_driver_at main.c)
target_link_libraries(test_driver_at PUBLIC RIOT::RIOT)
riot_target_depends(test_driver_at PUBLIC shell at at_urc_isr_medium)
riot_check_missing_modules(test_driver_at)
