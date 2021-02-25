add_library(riot_sys_flags INTERFACE)
target_include_directories(riot_sys_flags INTERFACE include)
target_link_libraries(riot_flags INTERFACE riot_sys_flags)

add_all_subdirectories()

add_riot_module(newlib INTERFACE)
target_link_libraries(riot_module_newlib INTERFACE c)

#if (CMAKE_C_COMPILER_ID STREQUAL "GNU")
#  get_filename_component(cc_path CMAKE_C_COMPILER DIRECTORY)
#  get_filename_component(sysroot cc_path DIRECTORY)
#  message(DEBUG "CMAKE_C_COMPILER: ${CMAKE_C_COMPILER}")
#  message(DEBUG "cc_path: ${cc_path}")
#  message(DEBUG "sysroot: ${sysroot}")
#find_path(NEWLIB_INCLUDE_DIR NAMES stddef.h
#  PATHS ${cc_path}
#  PATH_SUFFIXES include ${CMAKE_C_COMPILER_TARGET}/include include/newlib
#)
#message(VERBOSE "NEWLIB_INCLUDE_DIR: ${NEWLIB_INCLUDE_DIR}")
add_riot_module(newlib_nano INTERFACE)
target_compile_options(riot_module_newlib_nano INTERFACE -specs=nano.specs)
target_link_options(riot_module_newlib_nano INTERFACE -specs=nano.specs)

include(CheckIncludeFile)
check_include_file(newlib.h HAVE_NEWLIB_NANO -specs=nano.specs)

if (HAVE_NEWLIB_NANO)
  riot_module_depends(newlib newlib_nano)
endif()

add_riot_module(newlib_gnu_source INTERFACE)
target_compile_definitions(riot_module_newlib_gnu_source INTERFACE _GNU_SOURCE)
