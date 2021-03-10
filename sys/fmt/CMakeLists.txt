add_riot_module(fmt fmt.c)
add_riot_module(fmt_table table.c)
riot_module_depends(fmt_table fmt)
