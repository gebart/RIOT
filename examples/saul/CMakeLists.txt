cmake_minimum_required(VERSION 3.18)
project(saul)

set(RIOT_BASEDIR ${CMAKE_CURRENT_LIST_DIR}/../../)
list(APPEND CMAKE_MODULE_PATH "${RIOT_BASEDIR}/cmake" "${CMAKE_MODULE_PATH}")
include(RIOT)

# we want to use SAUL:
riot_use_module(saul_default)
# include the shell:
riot_use_module(shell shell_commands)
# additional modules for debugging:
riot_use_module(ps)

add_executable(saul main.c)
riot_configure_target(saul)
