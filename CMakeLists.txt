cmake_minimum_required(VERSION 3.14)
project(RIOT
  VERSION 2021.01
  HOMEPAGE_URL https://github.com/RIOT-OS/RIOT
  LANGUAGES C CXX ASM
  )

if (NOT BOARD)
  message(FATAL_ERROR "Define BOARD to select platform: cmake -DBOARD=foo")
endif ()

# Skip configuration of unit tests when RIOT is included as a subproject of another project
if (CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME)
  include(CTest)
endif ()

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS ON)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# RIOT::RIOT is the top level target which is meant to be used from the
# application to add a dependency on RIOT
add_library(RIOT INTERFACE)
add_library(RIOT::RIOT ALIAS RIOT)
target_link_libraries(RIOT INTERFACE
  RIOT::cpu
  RIOT::board
  RIOT::core
  RIOT::modules
  RIOT::flags
  )

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

# Create a RIOT module library target and set up the expected MODULE_x compiler defintion
function(add_riot_module internal_name exported_name library_kind)
  string(TOUPPER "${internal_name}" internal_name_uc)
  set(prefixed_name "riot_${internal_name}")
  add_library(${prefixed_name} ${library_kind} EXCLUDE_FROM_ALL ${ARGN})
  add_library(${prefixed_name}_module_flags INTERFACE)
  add_library(${exported_name} ALIAS ${prefixed_name})
  add_library(${exported_name}::module_flags ALIAS ${prefixed_name}_module_flags)
  target_compile_definitions(${prefixed_name}_module_flags INTERFACE MODULE_${internal_name_uc})
  if (library_kind STREQUAL "INTERFACE")
    target_link_libraries(${prefixed_name} INTERFACE RIOT::flags)
  else()
    target_link_libraries(${prefixed_name} PUBLIC RIOT::flags)
  endif()
endfunction()

# This replaces the USEMODULE += module pattern found in the old Makefile based build system
function(riot_use_module exported_name) # , exported_name2, exported_name3, ... (variadic)
  message("Adding ${ARGV} to riot_modules")
  target_link_libraries(riot_modules INTERFACE ${ARGV})
  set(module_flags_targets ${ARGV})
  list(TRANSFORM module_flags_targets APPEND "::module_flags")
  message("Adding ${module_flags_targets} to riot_modules_flags")
  target_link_libraries(riot_modules_flags INTERFACE ${module_flags_targets})
  get_target_property(target_type ${exported_name} TYPE)
  if (target_type STREQUAL "OBJECT_LIBRARY")
    target_sources(riot_modules INTERFACE $<TARGET_OBJECTS:${exported_name}>)
  endif()
endfunction()

if (NOT BOARD STREQUAL "native" OR NOT CMAKE_SYSTEM_NAME STREQUAL "Darwin")
  # Xcode linker does not support --gc-sections
  target_link_options(riot_flags INTERFACE LINKER:--gc-sections)
endif ()

#add_link_options(-nostartfiles)

#add_compile_options($<$<COMPILE_LANGUAGE:CXX>:-fno-use-cxa-atexit>)
#add_compile_options($<$<COMPILE_LANGUAGE:CXX>:-fno-rtti>)
#add_compile_options($<$<COMPILE_LANGUAGE:CXX>:-fno-exceptions>)
target_compile_options(riot_flags INTERFACE -fno-common)
target_compile_options(riot_flags INTERFACE -fdata-sections -ffunction-sections)
#if (CMAKE_C_COMPILER_ID STREQUAL GNU)
#  add_compile_options(-fstack-usage)
#  add_compile_options(-findirect-inlining)
#  add_compile_options(-finline-small-functions)
#endif ()

if (MSVC)
  add_compile_options(/W4)
  if (RIOT_WERROR)
    add_compile_options(/WX)
  endif ()
else ()
  # lots of warnings and all warnings as errors
  #-Wshadow -Wundef
  target_compile_options(riot_flags INTERFACE -Wall -Wextra -pedantic)
  if (RIOT_WERROR)
    target_compile_options(riot_flags INTERFACE -Werror)
  endif ()
  target_compile_options(riot_flags INTERFACE -Wdouble-promotion -Wconversion)
  target_compile_options(riot_flags INTERFACE -Wswitch-enum)
  target_compile_options(riot_flags INTERFACE -Wuninitialized -Winit-self)
  target_compile_options(riot_flags INTERFACE -Wformat=2)
  target_compile_options(riot_flags INTERFACE -fdiagnostics-color=always)
  if (CMAKE_C_COMPILER_ID STREQUAL GNU)
    target_compile_options(riot_flags INTERFACE -Wformat-truncation -Wformat-signedness)
  endif ()
  #add_compile_options($<$<COMPILE_LANGUAGE:CXX>:-Woverloaded-virtual>)
  #add_compile_options($<$<COMPILE_LANGUAGE:CXX>:-Weffc++>)
endif ()

add_subdirectory(core)
add_subdirectory(cpu)
add_subdirectory(boards)
add_subdirectory(drivers)
add_subdirectory(sys)

if (CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME AND BUILD_TESTING)
  add_subdirectory(tests)
endif ()
