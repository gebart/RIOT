add_executable(test_gnrc_sock_dns main.c)
target_link_libraries(test_gnrc_sock_dns PUBLIC RIOT::RIOT)
riot_target_depends(test_gnrc_sock_dns PUBLIC
    sock_dns
    gnrc_ipv6_default
    gnrc_ipv6_nib_dns

    # Only one interface used and it makes shell commands easier
    gnrc_netif_single

    auto_init_gnrc_netif
    shell
    shell_commands
    posix_inet)
riot_check_missing_modules(test_gnrc_sock_dns)

set(low_memory_boards nucleo-f334r8 msb-430 msb-430h)
if (BOARD IN_LIST low_memory_boards)
  if (CONFIG_GNRC_PKTBUF_SIZE)
    target_compile_definitions(test_gnrc_sock_dns PUBLIC
        CONFIG_GNRC_NETIF_IPV6_ADDRS_NUMOF=2
        GNRC_NETIF_IPV6_GROUPS_NUMOF=2
        CONFIG_GNRC_IPV6_NIB_NUMOF=1
        CONFIG_GNRC_IPV6_NIB_OFFL_NUMOF=1
    )
  else()
    target_compile_definitions(test_gnrc_sock_dns PUBLIC CONFIG_GNRC_PKTBUF_SIZE=512)
  endif()
endif()
