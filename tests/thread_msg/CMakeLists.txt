add_executable(test_thread_msg main.c)
target_link_libraries(test_thread_msg PUBLIC RIOT::RIOT)
riot_check_missing_modules(test_thread_msg)
