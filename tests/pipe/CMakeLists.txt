add_executable(test_pipe main.c)
target_link_libraries(test_pipe PUBLIC RIOT::RIOT)
riot_target_depends(test_pipe PUBLIC pipe)
riot_check_missing_modules(test_pipe)
