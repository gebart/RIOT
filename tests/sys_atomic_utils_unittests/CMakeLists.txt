add_executable(test_sys_atomic_utils_unittests main.c)
target_link_libraries(test_sys_atomic_utils_unittests PUBLIC RIOT::RIOT)
riot_target_depends(test_sys_atomic_utils_unittests PUBLIC
    embunit
    atomic_utils
    random # <-- used for input data to operate on
)
riot_check_missing_modules(test_sys_atomic_utils_unittests)
