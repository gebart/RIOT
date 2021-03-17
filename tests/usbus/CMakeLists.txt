add_executable(test_usbus main.c)
target_link_libraries(test_usbus PUBLIC RIOT::RIOT)
riot_target_depends(test_usbus PUBLIC embunit usbus usbdev_mock)
riot_check_missing_modules(test_usbus)
