add_executable(test_ssp main.c)
target_link_libraries(test_ssp PUBLIC RIOT::RIOT)
riot_target_depends(test_ssp PUBLIC ssp)
riot_check_missing_modules(test_ssp)
