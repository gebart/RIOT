add_riot_module(fmt OBJECT fmt.c)
add_riot_module(fmt_table OBJECT table.c)
riot_module_depends(fmt_table fmt)
