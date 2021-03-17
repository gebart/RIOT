add_executable(test_sntp main.c)
target_link_libraries(test_sntp PUBLIC RIOT::RIOT)
riot_target_depends(test_sntp PUBLIC sntp gnrc_ipv6_default auto_init_gnrc_netif gnrc_netdev_default shell shell_commands)
riot_check_missing_modules(test_sntp)
