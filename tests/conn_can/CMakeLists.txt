add_executable(test_conn_can main.c)
target_link_libraries(test_conn_can PUBLIC RIOT::RIOT)
riot_target_depends(test_conn_can PUBLIC shell shell_commands ps conn_can can_isotp conn_can_isotp_multi can_pm can_trx auto_init_can tja1042 ncv7356)
riot_check_missing_modules(test_conn_can)
