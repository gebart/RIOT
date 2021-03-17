add_executable(test_od main.c)
target_link_libraries(test_od PUBLIC RIOT::RIOT)
riot_target_depends(test_od PUBLIC od od_string)
riot_check_missing_modules(test_od)
