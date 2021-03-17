add_executable(test_malloc main.c)
target_link_libraries(test_malloc PUBLIC RIOT::RIOT)
riot_target_depends(test_malloc PUBLIC )
riot_check_missing_modules(test_malloc)
