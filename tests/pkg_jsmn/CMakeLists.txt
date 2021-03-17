add_executable(test_pkg_jsmn simple.c)
target_link_libraries(test_pkg_jsmn PUBLIC RIOT::RIOT)
riot_target_depends(test_pkg_jsmn PUBLIC jsmn)
riot_check_missing_modules(test_pkg_jsmn)
