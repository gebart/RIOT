add_executable(test_pkg_minmea main.c)
target_link_libraries(test_pkg_minmea PUBLIC RIOT::RIOT)
riot_target_depends(test_pkg_minmea PUBLIC minmea fmt)
riot_check_missing_modules(test_pkg_minmea)
