add_riot_module(drivers)
target_include_directories(riot_module_drivers INTERFACE include)
riot_module_depends(RIOT drivers)

add_all_subdirectories()
