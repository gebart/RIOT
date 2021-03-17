add_executable(test_usbus_hid main.c)
target_link_libraries(test_usbus_hid PUBLIC RIOT::RIOT)
riot_target_depends(test_usbus_hid PUBLIC usbus_hid)
riot_check_missing_modules(test_usbus_hid)
