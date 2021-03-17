add_executable(test_warn_conflict main.c)
target_link_libraries(test_warn_conflict PUBLIC RIOT::RIOT)
riot_target_depends(test_warn_conflict PUBLIC )
riot_check_missing_modules(test_warn_conflict)
