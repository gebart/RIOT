add_executable(test_libc_newlib main.c)
target_link_libraries(test_libc_newlib PUBLIC RIOT::RIOT)
riot_target_depends(test_libc_newlib PUBLIC embunit)
riot_check_missing_modules(test_libc_newlib)
