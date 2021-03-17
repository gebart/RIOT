add_executable(test_l2util main.c)
target_link_libraries(test_l2util PUBLIC RIOT::RIOT)
riot_target_depends(test_l2util PUBLIC embunit l2util)
riot_check_missing_modules(test_l2util)
