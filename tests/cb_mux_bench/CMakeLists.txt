add_executable(test_cb_mux_bench main.c)
target_link_libraries(test_cb_mux_bench PUBLIC RIOT::RIOT)
riot_target_depends(test_cb_mux_bench PUBLIC cb_mux )
riot_check_missing_modules(test_cb_mux_bench)
