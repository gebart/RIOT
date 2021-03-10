add_riot_module(ssp ssp.c)
target_compile_options(riot_module_ssp INTERFACE -fstack-protector)
target_link_options(riot_module_ssp INTERFACE -fstack-protector)
