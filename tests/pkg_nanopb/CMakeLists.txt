add_executable(test_pkg_nanopb main.c)
target_link_libraries(test_pkg_nanopb PUBLIC RIOT::RIOT)
riot_target_depends(test_pkg_nanopb PUBLIC )
riot_check_missing_modules(test_pkg_nanopb)
