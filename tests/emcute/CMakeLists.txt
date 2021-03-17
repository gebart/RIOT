add_executable(test_emcute main.c)
target_link_libraries(test_emcute PUBLIC RIOT::RIOT)
riot_target_depends(test_emcute PUBLIC
    auto_init_gnrc_netif
    gnrc_ipv6_default
    sock_udp

    # Only one interface used and it makes shell commands easier
    gnrc_netif_single

    emcute
    od
    shell
    shell_commands
    sock_util
)
target_compile_definitions(test_emcute PUBLIC "STDIO_UART_RX_BUFSIZE=512") # Adapt to SHELL_BUFSIZE in app
# TODO Add emcute config variables
target_compile_definitions(test_emcute PUBLIC "CONFIG_EMCUTE_TOPIC_MAXLEN=249") # 256 - 7
riot_check_missing_modules(test_emcute)
