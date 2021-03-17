add_executable(test_touch_dev main.c)
target_link_libraries(test_touch_dev PUBLIC RIOT::RIOT)
riot_target_depends(test_touch_dev PUBLIC stmpe811 touch_dev xtimer)
riot_check_missing_modules(test_touch_dev)
