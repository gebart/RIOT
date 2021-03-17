add_executable(test_nimble_l2cap main.c)
target_link_libraries(test_nimble_l2cap PUBLIC RIOT::RIOT)
riot_target_depends(test_nimble_l2cap PUBLIC xtimer shell shell_commands ps bluetil_ad)
riot_check_missing_modules(test_nimble_l2cap)
