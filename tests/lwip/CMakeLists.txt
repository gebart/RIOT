add_executable(test_lwip main.c)
target_link_libraries(test_lwip PUBLIC RIOT::RIOT)
riot_target_depends(test_lwip PUBLIC ipv4_addr lwip_arp lwip_ipv4 lwip_dhcp_auto ipv6_addr lwip_ipv6 lwip_ipv6_autoconfig lwip lwip_netdev lwip_udp lwip_tcp sock_async_event sock_ip sock_tcp sock_udp sock_util shell shell_commands ps od netdev_default lwip_ethernet)
riot_check_missing_modules(test_lwip)
