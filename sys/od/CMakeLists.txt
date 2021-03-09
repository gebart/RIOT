add_riot_module(od OBJECT od.c)
riot_module_depends(od fmt)
add_riot_module(od_string INTERFACE)
riot_module_depends(od_string od)
