add_executable(test_pkg_cmsis-nn main.c)
target_link_libraries(test_pkg_cmsis-nn PUBLIC RIOT::RIOT)
riot_target_depends(test_pkg_cmsis-nn PUBLIC cmsis-nn)
riot_check_missing_modules(test_pkg_cmsis-nn)
