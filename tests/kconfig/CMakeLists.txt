add_executable(test_kconfig main.c)
target_link_libraries(test_kconfig PUBLIC RIOT::RIOT)
riot_target_depends(test_kconfig PUBLIC external_module_1 external_module_2)
riot_check_missing_modules(test_kconfig)
