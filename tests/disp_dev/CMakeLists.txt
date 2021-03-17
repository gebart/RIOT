add_executable(test_disp_dev main.c)
target_link_libraries(test_disp_dev PUBLIC RIOT::RIOT)
riot_target_depends(test_disp_dev PUBLIC ili9341 disp_dev)
riot_check_missing_modules(test_disp_dev)
