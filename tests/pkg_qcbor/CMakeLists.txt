add_executable(test_pkg_qcbor main.c)
target_link_libraries(test_pkg_qcbor PUBLIC RIOT::RIOT)
riot_target_depends(test_pkg_qcbor PUBLIC qcbor embunit)
riot_check_missing_modules(test_pkg_qcbor)
