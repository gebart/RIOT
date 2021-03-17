add_executable(test_candev main.c)
target_link_libraries(test_candev PUBLIC RIOT::RIOT)
riot_target_depends(test_candev PUBLIC shell can isrpipe mcp2515)
riot_check_missing_modules(test_candev)
