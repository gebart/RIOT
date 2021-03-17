add_executable(test_pkg_hacl main.c)
target_link_libraries(test_pkg_hacl PUBLIC RIOT::RIOT)
riot_target_depends(test_pkg_hacl PUBLIC random embunit)
riot_check_missing_modules(test_pkg_hacl)
