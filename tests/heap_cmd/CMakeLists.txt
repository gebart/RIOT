add_executable(test_heap_cmd main.c)
target_link_libraries(test_heap_cmd PUBLIC RIOT::RIOT)
riot_target_depends(test_heap_cmd PUBLIC heap_cmd shell shell_commands ps)
riot_check_missing_modules(test_heap_cmd)
