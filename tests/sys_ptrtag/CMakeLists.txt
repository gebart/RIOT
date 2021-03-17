add_executable(test_sys_ptrtag main.c)
target_link_libraries(test_sys_ptrtag PUBLIC RIOT::RIOT)
riot_target_depends(test_sys_ptrtag PUBLIC )
riot_check_missing_modules(test_sys_ptrtag)
