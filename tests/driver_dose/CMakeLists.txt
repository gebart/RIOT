add_executable(test_driver_dose main.c)
target_link_libraries(test_driver_dose PUBLIC RIOT::RIOT)
riot_target_depends(test_driver_dose PUBLIC dose)
riot_check_missing_modules(test_driver_dose)
