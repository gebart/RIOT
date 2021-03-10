add_executable(test_shell main.c)
target_link_libraries(test_shell PUBLIC RIOT::RIOT)
riot_target_depends(test_shell PUBLIC app_metadata shell shell_commands ps)
