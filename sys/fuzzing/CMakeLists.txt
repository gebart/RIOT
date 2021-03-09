add_riot_module(fuzzing STATIC fuzzing.c netdev.c)
riot_module_depends(fuzzing netdev_test gnrc_netif gnrc_pktbuf_malloc)
