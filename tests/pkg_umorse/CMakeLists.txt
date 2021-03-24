add_executable(test_pkg_umorse main.c)
target_link_libraries(test_pkg_umorse PUBLIC RIOT::RIOT)
riot_target_depends(test_pkg_umorse PUBLIC umorse ztimer)
riot_check_missing_modules(test_pkg_umorse)
