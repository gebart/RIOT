add_executable(test_msg_avail main.c)
target_link_libraries(test_msg_avail PUBLIC RIOT::RIOT)
riot_target_depends(test_msg_avail PUBLIC )
riot_check_missing_modules(test_msg_avail)
