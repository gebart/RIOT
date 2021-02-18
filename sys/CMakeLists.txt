add_library(riot_sys INTERFACE)
add_library(RIOT::sys ALIAS riot_sys)
target_include_directories(riot_sys INTERFACE include)
