add_executable(test_lua_loader main.c)
target_link_libraries(test_lua_loader PUBLIC RIOT::RIOT)
riot_target_depends(test_lua_loader PUBLIC lua stdin)
riot_check_missing_modules(test_lua_loader)
