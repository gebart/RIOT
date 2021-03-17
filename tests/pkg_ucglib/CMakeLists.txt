add_executable(test_pkg_ucglib main.c)
target_link_libraries(test_pkg_ucglib PUBLIC RIOT::RIOT)
riot_target_depends(test_pkg_ucglib PUBLIC xtimer ucglib_sdl)
riot_check_missing_modules(test_pkg_ucglib)
