add_executable(test_devfs main.c)
target_link_libraries(test_devfs PUBLIC RIOT::RIOT)
riot_target_depends(test_devfs PUBLIC embunit vfs devfs devfs_random devfs_hwrng)
riot_check_missing_modules(test_devfs)
