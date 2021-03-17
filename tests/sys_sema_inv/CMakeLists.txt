add_executable(test_sys_sema_inv main.c)
target_link_libraries(test_sys_sema_inv PUBLIC RIOT::RIOT)
riot_target_depends(test_sys_sema_inv PUBLIC sema_inv xtimer)
riot_check_missing_modules(test_sys_sema_inv)
