add_riot_module(shell_commands shell_commands.c)

set(submodules
    cmd_app_metadata
    cmd_at30tse75x
    cmd_blacklist
    cmd_can
    cmd_ccnl
    cmd_cord_ep
    cmd_cryptoauthlib
    cmd_dfplayer
    cmd_disk
    cmd_fib
    cmd_gnrc_6ctx
    cmd_gnrc_6lo_frag_stats
    cmd_gnrc_icmpv6_echo
    cmd_gnrc_ipv6_frag_stats
    cmd_gnrc_ipv6_nib
    cmd_gnrc_netif
    cmd_gnrc_pktbuf
    cmd_gnrc_rpl
    cmd_heap
    cmd_i2c_scan
    cmd_loramac
    cmd_lwip_netif
    cmd_netstats_nb
    cmd_nimble_netif
    cmd_nimble_statconn
    cmd_openwsn
    cmd_pm
    cmd_ps
    cmd_random
    cmd_rtc
    cmd_rtt
    cmd_saul_reg
    cmd_sht1x
    cmd_sntp
    cmd_suit
    cmd_sys
    cmd_vfs
    cmd_whitelist
)
foreach(shell_command IN LISTS submodules)
  add_riot_module(shell_${shell_command} ${shell_command}.c)
  riot_module_depends(shell_${shell_command} shell)
endforeach()

function(riot_is_module_used_genex genex usemodule)
  string(MAKE_C_IDENTIFIER "${usemodule}" usemodule_id)
  set(${genex} "$<BOOL:$<TARGET_PROPERTY:INTERFACE_RIOT_MODULE_${usemodule_id}>>" PARENT_SCOPE)
endfunction()

function(riot_enable_shell_command_if_usemodule shell_command usemodule)
  riot_is_module_used_genex(genex ${usemodule})
  target_link_libraries(riot_module_shell_commands INTERFACE $<${genex}:riot_module_${shell_command}>)
  riot_module_depends(${shell_command} ${usemodule})
endfunction()

# sys shell commands are enabled by default
riot_module_depends(shell_commands shell_cmd_sys)

# Conditionally add the shell commands depending on the enabled modules
riot_enable_shell_command_if_usemodule(shell_cmd_app_metadata app_metadata)
riot_enable_shell_command_if_usemodule(shell_cmd_dfplayer dfplayer)
riot_enable_shell_command_if_usemodule(shell_cmd_disk mci)
riot_enable_shell_command_if_usemodule(shell_cmd_pm periph_pm)
riot_enable_shell_command_if_usemodule(shell_cmd_ps ps)
riot_enable_shell_command_if_usemodule(shell_cmd_heap heap_cmd)
riot_enable_shell_command_if_usemodule(shell_cmd_sht1x sht1x)
#riot_enable_shell_command_if_usemodule(shell_cmd_heap lpc2387) # WAT?
riot_enable_shell_command_if_usemodule(shell_cmd_random random)
riot_enable_shell_command_if_usemodule(shell_cmd_at30tse75x at30tse75x)
riot_enable_shell_command_if_usemodule(shell_cmd_gnrc_netif gnrc_netif)
riot_enable_shell_command_if_usemodule(shell_cmd_netstats_nb netstats_neighbor)
riot_enable_shell_command_if_usemodule(shell_cmd_fib fib)
riot_enable_shell_command_if_usemodule(shell_cmd_gnrc_ipv6_frag_stats gnrc_ipv6_ext_frag_stats)
riot_enable_shell_command_if_usemodule(shell_cmd_gnrc_ipv6_nib gnrc_ipv6_nib)
riot_enable_shell_command_if_usemodule(shell_cmd_whitelist gnrc_ipv6_whitelist)
riot_enable_shell_command_if_usemodule(shell_cmd_blacklist gnrc_ipv6_blacklist)
#riot_enable_shell_command_if_usemodule(shell_cmd_gnrc_icmpv6_echo gnrc_icmpv6_echo xtimer)
riot_enable_shell_command_if_usemodule(shell_cmd_gnrc_pktbuf gnrc_pktbuf_cmd)
riot_enable_shell_command_if_usemodule(shell_cmd_gnrc_rpl gnrc_rpl)
riot_enable_shell_command_if_usemodule(shell_cmd_gnrc_6ctx gnrc_sixlowpan_ctx)
riot_enable_shell_command_if_usemodule(shell_cmd_gnrc_6lo_frag_stats gnrc_sixlowpan_frag_stats)
riot_enable_shell_command_if_usemodule(shell_cmd_saul_reg saul_reg)
riot_enable_shell_command_if_usemodule(shell_cmd_ccnl ccn-lite-utils)
riot_enable_shell_command_if_usemodule(shell_cmd_sntp sntp)
riot_enable_shell_command_if_usemodule(shell_cmd_vfs vfs)
riot_enable_shell_command_if_usemodule(shell_cmd_can conn_can)
riot_enable_shell_command_if_usemodule(shell_cmd_cord_ep cord_ep)
riot_enable_shell_command_if_usemodule(shell_cmd_openwsn openwsn)
riot_enable_shell_command_if_usemodule(shell_cmd_lwip_netif lwip_netif)
riot_enable_shell_command_if_usemodule(shell_cmd_rtc periph_rtc)
riot_enable_shell_command_if_usemodule(shell_cmd_rtt rtt_cmd)
riot_enable_shell_command_if_usemodule(shell_cmd_i2c_scan i2c_scan)
riot_enable_shell_command_if_usemodule(shell_cmd_loramac semtech-loramac)
riot_enable_shell_command_if_usemodule(shell_cmd_nimble_netif nimble_netif)
riot_enable_shell_command_if_usemodule(shell_cmd_nimble_statconn nimble_statconn)
riot_enable_shell_command_if_usemodule(shell_cmd_suit suit_transport_coap)
riot_enable_shell_command_if_usemodule(shell_cmd_cryptoauthlib cryptoauthlib)
