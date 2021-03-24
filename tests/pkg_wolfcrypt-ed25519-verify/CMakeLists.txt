add_executable(test_pkg_wolfcrypt-ed25519-verify main.c)
target_link_libraries(test_pkg_wolfcrypt-ed25519-verify PUBLIC RIOT::RIOT)
riot_target_depends(test_pkg_wolfcrypt-ed25519-verify PUBLIC wolfssl wolfcrypt wolfcrypt_ed25519)
riot_check_missing_modules(test_pkg_wolfcrypt-ed25519-verify)
