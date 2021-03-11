include_guard(GLOBAL)
include(riot_functions)
include(riot_config)

# RIOT::RIOT is the top level target which is meant to be used from the
# application to add a dependency on RIOT
add_riot_module(RIOT)
add_library(RIOT::RIOT ALIAS riot_module_RIOT)
target_compile_definitions(riot_module_RIOT INTERFACE RIOT_VERSION="${PROJECT_VERSION}")

# Abstract interface modules to allow the user to select the stdio implementation
add_riot_module(stdio)
riot_module_depends(RIOT stdio)
add_riot_module(stdin)
# Note that in order to reduce binary bloat, stdin must be explicitly enabled
# in each application that uses terminal input

if (NOT RIOT_STDIO)
  message(FATAL_ERROR "No stdio implementation specified")
endif()
riot_module_depends(stdio ${RIOT_STDIO})

if (NOT BOARD STREQUAL "native")
  # The libc module is an abstract interface that allows us to select C library
  # implementation in the build configuration
  add_riot_module(libc)
  riot_module_depends(RIOT libc)

  if (NOT RIOT_LIBC)
    message(FATAL_ERROR "No libc implementation specified")
  endif()
  riot_module_depends(libc ${RIOT_LIBC})

  if (NOT RIOT_SYSCALLS)
    message(FATAL_ERROR "No syscalls implementation specified")
  endif()
  riot_module_depends(libc ${RIOT_SYSCALLS})
endif()

if (RIOT_USEMODULE_EXTRA)
  riot_module_depends(RIOT ${RIOT_USEMODULE_EXTRA})
endif()
