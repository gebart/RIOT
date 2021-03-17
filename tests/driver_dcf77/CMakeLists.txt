add_executable(test_driver_dcf77 main.c)
target_link_libraries(test_driver_dcf77 PUBLIC RIOT::RIOT)
riot_target_depends(test_driver_dcf77 PUBLIC dcf77)
riot_check_missing_modules(test_driver_dcf77)
