add_executable(test_netdev_test main.c)
target_link_libraries(test_netdev_test PUBLIC RIOT::RIOT)
riot_target_depends(test_netdev_test PUBLIC gnrc gnrc_neterr gnrc_netif netdev_eth netdev_test od)
riot_check_missing_modules(test_netdev_test)
