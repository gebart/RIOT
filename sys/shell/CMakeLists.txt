add_riot_module(shell shell.c)
riot_module_depends(shell stdin)

if (BOARD STREQUAL "native")
  set(shutdown_on_exit_default ON)
else()
  set(shutdown_on_exit_default OFF)
endif()
option(SHELL_SHUTDOWN_ON_EXIT "Shutdown RIOT on shell exit" ${shutdown_on_exit_default})
option(SHELL_NO_ECHO "Disable echo" OFF)
option(SHELL_NO_PROMPT "Disable prompt" OFF)

add_subdirectory(commands)
