add_executable(test_mcuboot main.c)
target_link_libraries(test_mcuboot PUBLIC RIOT::RIOT)
riot_target_depends(test_mcuboot PUBLIC )
riot_check_missing_modules(test_mcuboot)
