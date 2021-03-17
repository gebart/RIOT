add_executable(test_pkg_fatfs main.c)
target_link_libraries(test_pkg_fatfs PUBLIC RIOT::RIOT)
riot_target_depends(test_pkg_fatfs PUBLIC shell fatfs_diskio_mtd mtd mtd_sdcard)
riot_check_missing_modules(test_pkg_fatfs)
