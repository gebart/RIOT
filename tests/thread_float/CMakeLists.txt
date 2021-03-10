add_executable(test_thread_float main.c)
target_link_libraries(test_thread_float PUBLIC RIOT::RIOT)
riot_target_depends(test_thread_float PUBLIC printf_float xtimer)
