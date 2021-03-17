add_executable(test_buttons main.c)
target_link_libraries(test_buttons PUBLIC RIOT::RIOT)
riot_target_depends(test_buttons PUBLIC )
riot_check_missing_modules(test_buttons)
