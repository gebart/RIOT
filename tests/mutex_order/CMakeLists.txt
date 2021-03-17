add_executable(test_mutex_order main.c)
target_link_libraries(test_mutex_order PUBLIC RIOT::RIOT)
riot_target_depends(test_mutex_order PUBLIC )
riot_check_missing_modules(test_mutex_order)
