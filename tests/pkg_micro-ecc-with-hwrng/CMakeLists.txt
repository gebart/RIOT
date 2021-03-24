add_executable(test_pkg_micro-ecc-with-hwrng main.c)
target_link_libraries(test_pkg_micro-ecc-with-hwrng PUBLIC RIOT::RIOT)
riot_target_depends(test_pkg_micro-ecc-with-hwrng PUBLIC micro-ecc)
riot_check_missing_modules(test_pkg_micro-ecc-with-hwrng)
