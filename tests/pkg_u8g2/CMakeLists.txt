add_executable(test_pkg_u8g2 main.c)
target_link_libraries(test_pkg_u8g2 PUBLIC RIOT::RIOT)
riot_target_depends(test_pkg_u8g2 PUBLIC u8g2 xtimer u8g2_utf8 u8g2_sdl)
riot_check_missing_modules(test_pkg_u8g2)
