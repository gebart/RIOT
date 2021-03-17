add_executable(test_float main.c)
target_link_libraries(test_float PUBLIC RIOT::RIOT)
riot_target_depends(test_float PUBLIC )
riot_check_missing_modules(test_float)
