add_executable(test_unittests main.c)
target_link_libraries(test_unittests PUBLIC RIOT::RIOT)
riot_target_depends(test_unittests PUBLIC embunit)
riot_check_missing_modules(test_unittests)
