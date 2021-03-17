add_executable(test_slip main.c)
target_link_libraries(test_slip PUBLIC RIOT::RIOT)
riot_target_depends(test_slip PUBLIC auto_init_gnrc_netif gnrc gnrc_pktdump gnrc_txtsnd slipdev shell shell_commands)
riot_check_missing_modules(test_slip)
