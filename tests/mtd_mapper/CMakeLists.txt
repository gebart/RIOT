add_executable(test_mtd_mapper main.c)
target_link_libraries(test_mtd_mapper PUBLIC RIOT::RIOT)
riot_target_depends(test_mtd_mapper PUBLIC mtd_mapper embunit)
riot_check_missing_modules(test_mtd_mapper)
