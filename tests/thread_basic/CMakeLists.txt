add_executable(test_thread_basic main.c)
target_link_libraries(test_thread_basic PUBLIC RIOT::RIOT)
riot_check_missing_modules(test_thread_basic)
