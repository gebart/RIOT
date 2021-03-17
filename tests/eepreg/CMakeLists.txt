add_executable(test_eepreg main.c)
target_link_libraries(test_eepreg PUBLIC RIOT::RIOT)
riot_target_depends(test_eepreg PUBLIC eepreg)
riot_check_missing_modules(test_eepreg)
