add_executable(test_gnrc_tcp main.c)
target_link_libraries(test_gnrc_tcp PUBLIC RIOT::RIOT)
riot_target_depends(test_gnrc_tcp PUBLIC 
    auto_init_gnrc_netif
    gnrc_ipv6_default
    gnrc_tcp
    gnrc_pktbuf_cmd

    # Only one interface used and it makes shell commands easier
    gnrc_netif_single

    shell
    shell_commands
    od
)
riot_check_missing_modules(test_gnrc_tcp)
