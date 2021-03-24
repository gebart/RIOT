add_executable(test_pkg_libcose main.c)
target_link_libraries(test_pkg_libcose PUBLIC RIOT::RIOT)
riot_target_depends(test_pkg_libcose PUBLIC
    libcose
    libcose_crypt
    memarray
    embunit
)
riot_check_missing_modules(test_pkg_libcose)
