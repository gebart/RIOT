include_guard(GLOBAL)
include(riot_functions)
include(riot_config)

# These modules should be defined in every cpu and board CMakeLists.txt
riot_use_module(cpu board)

# The libc module is an abstract interface that allows us to select C library
# implementation in the build configuration
add_riot_module(libc INTERFACE)
riot_use_module(libc)

# Abstract interface modules to allow the user to select the stdio implementation
add_riot_module(stdio INTERFACE)
riot_use_module(stdio)
add_riot_module(stdin INTERFACE)
# Note that in order to reduce binary bloat, stdin must be explicitly enabled
# in each application that uses terminal input

if (NOT RIOT_LIBC)
  message(FATAL_ERROR "No libc implementation specified")
endif()
riot_module_depends(libc ${RIOT_LIBC})

if (NOT RIOT_SYSCALLS)
  message(FATAL_ERROR "No syscalls implementation specified")
endif()
riot_module_depends(libc ${RIOT_SYSCALLS})

if (NOT RIOT_STDIO)
  message(FATAL_ERROR "No stdio implementation specified")
endif()
riot_module_depends(stdio ${RIOT_STDIO})

if (RIOT_USEMODULE_EXTRA)
  riot_use_module(${RIOT_USEMODULE_EXTRA})
endif()

