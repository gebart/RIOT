add_executable(test_pkg_libb2 main.c)
target_link_libraries(test_pkg_libb2 PUBLIC RIOT::RIOT)
riot_target_depends(test_pkg_libb2 PUBLIC embunit)
riot_check_missing_modules(test_pkg_libb2)
