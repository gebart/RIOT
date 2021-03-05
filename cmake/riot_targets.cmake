include_guard(GLOBAL)

include(riot_functions)

# RIOT::RIOT is the top level target which is meant to be used from the
# application to add a dependency on RIOT
add_library(RIOT INTERFACE)
add_library(RIOT::RIOT ALIAS RIOT)

# RIOT::flags contains all compiler flags that are necessary in all RIOT
# related code. There are many occurrences of conditional compilation in RIOT,
# even within API headers, so certain preprocessor definitions need to be
# consistent across translation units to avoid accidental ABI mismatches
# Target hardware configuration flags are also found in RIOT::flags, e.g.
# -mcpu, -march etc.
add_library(riot_flags INTERFACE)
add_library(RIOT::flags ALIAS riot_flags)

# RIOT::modules is a target which depends on all _enabled_ modules of the
# build. This is similar to the purpose of USEMODULE in the old Makefile based
# build system.
add_library(riot_modules INTERFACE)
add_library(RIOT::modules ALIAS riot_modules)
# propagate any object modules to the consumers of this target
target_sources(RIOT INTERFACE $<TARGET_PROPERTY:riot_modules,INTERFACE_SOURCES>)

# RIOT::modules::flags contain the compiler flags of all _enabled_ modules of
# the build. This is used to avoid circular dependencies because every enabled
# module needs to be able to see all preprocessor definitions for all other
# enabled modules, but not necessarily linking with the objects of those
# modules.
add_library(riot_modules_flags INTERFACE)
add_library(RIOT::modules::flags ALIAS riot_modules_flags)

# RIOT::flags depends on the enabled module flags too for consistency.
target_link_libraries(riot_flags INTERFACE riot_modules_flags)
target_compile_definitions(riot_flags INTERFACE RIOT_VERSION="2021.04-devel-cmake")

target_link_libraries(RIOT INTERFACE
  RIOT::core
  RIOT::modules
  RIOT::flags
  )


