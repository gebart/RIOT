add_riot_module(sys_bus OBJECT sys_bus_init.c)
riot_module_depends(sys_bus core_msg_bus)
