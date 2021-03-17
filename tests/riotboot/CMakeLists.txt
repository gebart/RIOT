add_executable(test_riotboot main.c)
target_link_libraries(test_riotboot PUBLIC RIOT::RIOT)
riot_target_depends(test_riotboot PUBLIC riotboot_slot shell shell_commands)
riot_check_missing_modules(test_riotboot)
