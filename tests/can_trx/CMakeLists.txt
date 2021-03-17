add_executable(test_can_trx main.c)
target_link_libraries(test_can_trx PUBLIC RIOT::RIOT)
riot_target_depends(test_can_trx PUBLIC shell shell_commands ps can_trx tja1042 ncv7356)
riot_check_missing_modules(test_can_trx)
