add_executable(test_pkg_micro-ecc main.c)
target_link_libraries(test_pkg_micro-ecc PUBLIC RIOT::RIOT)
riot_target_depends(test_pkg_micro-ecc PUBLIC micro-ecc hashes)
riot_check_missing_modules(test_pkg_micro-ecc)
