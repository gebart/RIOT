add_riot_module(can OBJECT device.c dll.c pkt.c router.c)
riot_module_depends(can can_raw memarray)

add_riot_module(can_isotp OBJECT isotp/isotp.c)
riot_module_depends(can_isotp can xtimer gnrc_pktbuf)

add_riot_module(conn_can OBJECT conn/raw.c conn/isotp.c)
riot_module_depends(conn_can can can_mbox xtimer)

add_riot_module(conn_can_isotp_multi INTERFACE)

add_riot_module(can_mbox INTERFACE)
riot_module_depends(can_mbox core_mbox)

add_riot_module(can_pm INTERFACE)
add_riot_module(can_raw INTERFACE)
