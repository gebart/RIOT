add_library(riot_drivers INTERFACE)
add_library(RIOT::drivers ALIAS riot_drivers)
target_include_directories(riot_drivers INTERFACE include)
