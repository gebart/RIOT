add_executable(test_pkg_lvgl main.c)
target_link_libraries(test_pkg_lvgl PUBLIC RIOT::RIOT)
riot_target_depends(test_pkg_lvgl PUBLIC lvgl_contrib ili9341)
riot_check_missing_modules(test_pkg_lvgl)
