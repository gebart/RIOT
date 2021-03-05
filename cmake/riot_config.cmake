include_guard(GLOBAL)
# Global configuration variables that can be assumed to be available in all
# RIOT project configurations

option(RIOT_DEVELHELP "Enable helpful developer settings. This will add some extra checks which indicate programmer mistakes and add some extra debug output in various places." OFF)

option(RIOT_WERROR "Build with all warnings as errors" OFF)

set(RIOT_USEMODULE_EXTRA "" CACHE STRING "Additional modules to enable in this build")

set(RIOT_LIBC "newlib" CACHE STRING
  "Specifies which libc implementation to use. If unsure, say \"newlib\"")
set(RIOT_SYSCALLS "${RIOT_LIBC}_syscalls_default" CACHE STRING
  "Select which syscall implementation to use. If unsure, say \"${RIOT_LIBC}_syscalls_default\"")
set(RIOT_STDIO "stdio_uart" CACHE STRING
  "Selects which standard input/output module will be used for the default console output (printf etc.) If unsure, say \"stdio_uart\"")
set_property(CACHE RIOT_STDIO PROPERTY STRINGS
  "stdio_uart"
  "stdio_null"
  "stdio_native"
  "stdio_semihosting"
  "stdio_cdc_acm"
  "stdio_ethos"
  "stdio_rtt"
  "slipdev_stdio"
)

if (RIOT_SYSCALLS MATCHES "^\(.*\)_syscalls_default$")
  if (NOT RIOT_LIBC STREQUAL CMAKE_MATCH_1)
    message(NOTICE "Mismatched syscalls implementation ${RIOT_SYSCALLS} against libc ${RIOT_LIBC}, adjusting to ${RIOT_LIBC}_syscalls_default")
    get_property(helpstring CACHE RIOT_SYSCALLS PROPERTY HELPSTRING)
    set(RIOT_SYSCALLS "${RIOT_LIBC}_syscalls_default" CACHE STRING "${helpstring}" FORCE)
  endif()
endif()

