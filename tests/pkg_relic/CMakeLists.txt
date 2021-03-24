add_executable(test_pkg_relic main.c)
target_link_libraries(test_pkg_relic PUBLIC RIOT::RIOT)
riot_target_depends(test_pkg_relic PUBLIC relic embunit)
riot_check_missing_modules(test_pkg_relic)
