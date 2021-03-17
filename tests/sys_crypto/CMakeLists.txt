add_executable(test_sys_crypto main.c)
target_link_libraries(test_sys_crypto PUBLIC RIOT::RIOT)
riot_target_depends(test_sys_crypto PUBLIC embunit crypto_3des cipher_modes)
riot_check_missing_modules(test_sys_crypto)
