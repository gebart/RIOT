add_executable(test_board_calliope-mini main.c)
target_link_libraries(test_board_calliope-mini PUBLIC RIOT::RIOT)
riot_target_depends(test_board_calliope-mini PUBLIC mini)
riot_check_missing_modules(test_board_calliope-mini)
