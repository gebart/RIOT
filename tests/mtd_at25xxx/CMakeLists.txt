add_executable(test_mtd_at25xxx main.c)
target_link_libraries(test_mtd_at25xxx PUBLIC RIOT::RIOT)
riot_target_depends(test_mtd_at25xxx PUBLIC mtd_at25xxx embunit)
riot_check_missing_modules(test_mtd_at25xxx)
