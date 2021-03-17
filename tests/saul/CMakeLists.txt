add_executable(test_saul main.c)
target_link_libraries(test_saul PUBLIC RIOT::RIOT)
riot_target_depends(test_saul PUBLIC saul_default xtimer)
riot_check_missing_modules(test_saul)
