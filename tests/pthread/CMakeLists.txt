add_executable(test_pthread main.c)
target_link_libraries(test_pthread PUBLIC RIOT::RIOT)
riot_target_depends(test_pthread PUBLIC posix_headers pthread)
riot_check_missing_modules(test_pthread)
