add_executable(test_cb_mux main.c)
target_link_libraries(test_cb_mux PUBLIC RIOT::RIOT)
riot_target_depends(test_cb_mux PUBLIC cb_mux)
riot_check_missing_modules(test_cb_mux)
