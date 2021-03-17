add_executable(test_rmutex main.c)
target_link_libraries(test_rmutex PUBLIC RIOT::RIOT)
riot_target_depends(test_rmutex PUBLIC )
riot_check_missing_modules(test_rmutex)
