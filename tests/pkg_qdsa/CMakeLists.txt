add_executable(test_pkg_qdsa main.c)
target_link_libraries(test_pkg_qdsa PUBLIC RIOT::RIOT)
riot_target_depends(test_pkg_qdsa PUBLIC random embunit)
riot_check_missing_modules(test_pkg_qdsa)
