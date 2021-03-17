add_executable(test_events main.c)
target_link_libraries(test_events PUBLIC RIOT::RIOT)
riot_target_depends(test_events PUBLIC event_callback event_timeout)
riot_check_missing_modules(test_events)
