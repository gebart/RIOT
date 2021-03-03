add_riot_module(isrpipe OBJECT isrpipe.c)
riot_module_depends(isrpipe tsrb)

add_subdirectory(read_timeout)
