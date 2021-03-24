add_executable(test_pkg_openwsn main.c)
target_link_libraries(test_pkg_openwsn PUBLIC RIOT::RIOT)
riot_target_depends(test_pkg_openwsn PUBLIC
    openwsn
    openwsn_openstack
    sock_udp
    sock_async
    # Optional OpenWSN Modules
    ## Enable Constrained Join Protocol (CoJP)
    openwsn_cjoin
    ## Enable 6lowpan fragmentation
    openwsn_6lo_fragmentation
    ## Enable link layer security
    # openwsn_iee802154e_security
    ## Enable MSF dynamic slot allocation
    # openwsn_adaptive_msf
    ## Optional Module, but required for root nodes
    # openwsn_serial
    # Optional Modules
    openwsn_leds
    openwsn_debugpins
    # stdio_null

    ## Test application Modules
    ps od_string shell shell_commands
    # ztimer is used instead of xtimer because it's a dependency of some
    # OpenWSN modules.
    ztimer_usec
)
riot_check_missing_modules(test_pkg_openwsn)
# TODO OpenWSN configuration
