add_executable(test_embunit main.c)
target_link_libraries(test_embunit PUBLIC RIOT::RIOT)
riot_target_depends(test_embunit PUBLIC embunit)
riot_check_missing_modules(test_embunit)
