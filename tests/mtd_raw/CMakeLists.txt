add_executable(test_mtd_raw main.c)
target_link_libraries(test_mtd_raw PUBLIC RIOT::RIOT)
riot_target_depends(test_mtd_raw PUBLIC shell shell_commands od mtd mtd_write_page)
riot_check_missing_modules(test_mtd_raw)
