add_executable(test_pkg_spiffs main.c)
target_link_libraries(test_pkg_spiffs PUBLIC RIOT::RIOT)
riot_target_depends(test_pkg_spiffs PUBLIC spiffs embunit)
riot_check_missing_modules(test_pkg_spiffs)
