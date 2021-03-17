add_executable(test_test_tools main.c)
target_link_libraries(test_test_tools PUBLIC RIOT::RIOT)
riot_target_depends(test_test_tools PUBLIC shell dummy_thread)
riot_check_missing_modules(test_test_tools)
