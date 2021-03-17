add_executable(test_driver_cc110x main.c)
target_link_libraries(test_driver_cc110x PUBLIC RIOT::RIOT)
# The MSB-A2 uses the CC1100. New boards use CC1101
set(RIOT_TEST_DRIVER_CC110X_MODEL "cc1100" CACHE STRING "The specific device to build test_driver_cc110x for")
mark_as_advanced(RIOT_TEST_DRIVER_CC110X_MODEL)
riot_target_depends(test_driver_cc110x PUBLIC
    # This test will rely on a human interacting with the shell, so we better add
    # the shell and some commands
    shell shell_commands
    # Add a network stack and auto init of the network devices
    gnrc auto_init_gnrc_netif gnrc_ipv6_router_default
    # checking current state of network threads can be useful for testing/debuggig
    ps
    # txtsnd can be useful to test lower layers
    gnrc_txtsnd auto_init_gnrc_netif
    # Activate ICMPv6 error messages
    gnrc_icmpv6_error
    # This application dumps received packets to STDIO using the pktdump module
    gnrc_pktdump
    # Additional networking modules that can be dropped if not needed
    gnrc_icmpv6_echo
    # Some statistics could also be helpful
    netstats_l2 netstats_ipv6
)
riot_check_missing_modules(test_driver_cc110x)
