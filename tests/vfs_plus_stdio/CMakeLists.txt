add_executable(test_vfs_plus_stdio main.c)
target_link_libraries(test_vfs_plus_stdio PUBLIC RIOT::RIOT)
riot_target_depends(test_vfs_plus_stdio PUBLIC vfs fmt)
riot_check_missing_modules(test_vfs_plus_stdio)
