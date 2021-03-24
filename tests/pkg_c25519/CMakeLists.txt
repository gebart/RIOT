add_executable(test_pkg_c25519 main.c)
target_link_libraries(test_pkg_c25519 PUBLIC RIOT::RIOT)
riot_target_depends(test_pkg_c25519 PUBLIC c25519 embunit random)
riot_check_missing_modules(test_pkg_c25519)
