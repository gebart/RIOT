add_riot_module(ssp OBJECT ssp.c)
target_compile_options(riot_module_ssp_module_flags INTERFACE -fstack-protector)
target_link_options(riot_module_ssp_module_flags INTERFACE -fstack-protector)
