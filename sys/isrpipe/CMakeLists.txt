add_riot_module(isrpipe isrpipe.c)
riot_module_depends(isrpipe tsrb)

add_riot_module(isrpipe_read_timeout read_timeout/read_timeout.c)
riot_module_depends(isrpipe_read_timeout isrpipe xtimer)
