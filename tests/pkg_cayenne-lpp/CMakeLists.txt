add_executable(test_pkg_cayenne-lpp main.c)
target_link_libraries(test_pkg_cayenne-lpp PUBLIC RIOT::RIOT)
riot_target_depends(test_pkg_cayenne-lpp PUBLIC )
riot_check_missing_modules(test_pkg_cayenne-lpp)
