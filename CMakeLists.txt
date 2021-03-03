cmake_minimum_required(VERSION 3.18)
project(RIOT
  VERSION 2021.01
  HOMEPAGE_URL https://github.com/RIOT-OS/RIOT
  LANGUAGES C CXX ASM
  )

if (NOT BOARD)
  message(FATAL_ERROR "Define BOARD to select platform: cmake -DBOARD=foo")
endif ()

set(BOARD "${BOARD}" CACHE STRING "Which board type the application should run on")

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
target_link_libraries(riot_flags INTERFACE RIOT::cpu::config RIOT::board::config)

# Convert the given string to an uppercase C symbol
function(uppercase_c_symbol out_var in_var)
  string(TOUPPER "${in_var}" upper)
  string(MAKE_C_IDENTIFIER "${upper}" sym)
  set(${out_var} "${sym}" PARENT_SCOPE)
endfunction()

if (BOARD)
  uppercase_c_symbol(BOARDDEF ${BOARD})
  target_compile_definitions(riot_flags INTERFACE "BOARD_${BOARDDEF}=\"${BOARD}\"" "RIOT_BOARD=BOARD_${BOARDDEF}")
endif()
if (CPU)
  uppercase_c_symbol(CPUDEF ${CPU})
  target_compile_definitions(riot_flags INTERFACE "CPU_${CPUDEF}=\"${CPU}\"" "RIOT_CPU=CPU_${CPUDEF}")
endif()
if (MCU)
  uppercase_c_symbol(MCUDEF ${MCU})
  target_compile_definitions(riot_flags INTERFACE "MCU_${MCUDEF}=\"${MCU}\"" "RIOT_MCU=MCU_${MCUDEF}")
endif()
target_compile_definitions(riot_flags INTERFACE "RIOT_APPLICATION=\"${CMAKE_PROJECT_NAME}\"") #TODO allow multiple application names in one project
if (CPU_CORE)
  uppercase_c_symbol(CPU_COREDEF ${CPU_CORE})
  target_compile_definitions(riot_flags INTERFACE "CPU_CORE_${CPU_COREDEF}")
endif()
if (CPU_MODEL)
  uppercase_c_symbol(CPU_MODELDEF ${CPU_MODEL})
  target_compile_definitions(riot_flags INTERFACE "CPU_MODEL_${CPU_MODELDEF}")
endif()


option(RIOT_WERROR "Build with all warnings as errors" ON)

# Create a RIOT module library target and set up the expected MODULE_x compiler defintion
function(add_riot_module module_name library_kind)
  string(TOUPPER "${module_name}" module_name_uc)
  set(prefixed_name "riot_module_${module_name}")
  message(DEBUG "Creating RIOT module ${prefixed_name}")
  if (library_kind STREQUAL "INTERFACE")
    add_library(${prefixed_name} ${library_kind} ${ARGN})
    target_link_libraries(${prefixed_name} INTERFACE RIOT::flags)
  else()
    add_library(${prefixed_name} ${library_kind} EXCLUDE_FROM_ALL ${ARGN})
    target_link_libraries(${prefixed_name} PUBLIC RIOT::flags)
  endif()
  add_library(${prefixed_name}_module_flags INTERFACE)
  target_compile_definitions(${prefixed_name}_module_flags INTERFACE MODULE_${module_name_uc})
  set_property(TARGET ${prefixed_name} APPEND PROPERTY INTERFACE_RIOT_USEMODULE ${module_name})
endfunction()

# This replaces the USEMODULE += module pattern found in the old Makefile based build system
function(riot_use_module module_name) # , module_name2, module_name3, ... (variadic)
  message(VERBOSE "Adding RIOT modules: ${ARGV}")
  riot_internal_module_target_depends(RIOT riot_modules_flags ${ARGV})
endfunction()

# Create dependencies between modules
function(riot_module_depends module) # , [dependencies]
  message(DEBUG "Adding dependencies to ${module}")
  riot_internal_module_target_depends(riot_module_${module} riot_module_${module}_module_flags ${ARGN})
endfunction()

# Internal helper method to avoid duplicating the configuration steps
function(riot_internal_module_target_depends module_target module_flags_target) # , [dependencies] 
  set(dependencies_targets ${ARGN})
  list(TRANSFORM dependencies_targets PREPEND "riot_module_")
  target_link_libraries(${module_target} INTERFACE ${dependencies_targets})
  set(dependencies_module_flags_targets ${dependencies_targets})
  list(TRANSFORM dependencies_module_flags_targets APPEND "_module_flags")
  target_link_libraries(${module_flags_target} INTERFACE ${dependencies_module_flags_targets})
  list(APPEND CMAKE_MESSAGE_INDENT "  ")
  foreach (dependency IN LISTS ARGN)
    message(DEBUG "${module_target} -> ${dependency}")
    target_sources(${module_target} INTERFACE $<$<STREQUAL:$<TARGET_PROPERTY:riot_module_${dependency},TYPE>,OBJECT_LIBRARY>:$<TARGET_OBJECTS:riot_module_${dependency}>>)
    set_property(TARGET ${module_target} APPEND PROPERTY INTERFACE_RIOT_USEMODULE ${dependency})
  endforeach()
  list(POP_BACK CMAKE_MESSAGE_INDENT)
endfunction()

# Enter all immediate subdirectories which have a CMakeLists.txt file
macro(add_all_subdirectories)
  file(GLOB cmakelists */CMakeLists.txt)
  foreach (cmakelist IN LISTS cmakelists)
    get_filename_component(subdir "${cmakelist}" DIRECTORY)
    add_subdirectory(${subdir})
  endforeach()
endmacro()

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

set(RIOT_LIBC "newlib" CACHE STRING "Specifies which libc implementation to use. If unsure, say \"newlib\"")
set(RIOT_SYSCALLS "${RIOT_LIBC}_syscalls_default" CACHE STRING "Select which syscall implementation to use. If unsure, say \"${RIOT_LIBC}_syscalls_default\"")
set(RIOT_STDIO "stdio_uart" CACHE STRING "Selects which standard input/output module will be used for the default console output (printf etc.) If unsure, say \"stdio_uart\"")
set_property(CACHE RIOT_STDIO PROPERTY STRINGS
  stdio_uart
  stdio_null
  stdio_native
  stdio_semihosting
  stdio_cdc_acm
  stdio_ethos
  stdio_rtt
  slipdev_stdio
)


if (CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME AND BUILD_TESTING)
  add_subdirectory(tests)
endif ()

set(RIOT_USEMODULE_EXTRA "" CACHE STRING "Additional modules to enable in this build")

if (RIOT_USEMODULE_EXTRA)
  riot_use_module(${RIOT_USEMODULE_EXTRA})
endif()

get_target_property(usemodule RIOT INTERFACE_RIOT_USEMODULE)
message(VERBOSE "Used RIOT modules: ${usemodule}")

add_riot_module(libc INTERFACE)
riot_use_module(libc)
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
# Abstract interface module to allow the user to select the stdio implementation
add_riot_module(stdio INTERFACE)
riot_use_module(stdio)
# Add dependency on the selected stdio implementation.
riot_module_depends(stdio ${RIOT_STDIO})

add_riot_module(stdin INTERFACE)
if (RIOT_STDIO STREQUAL "stdio_uart")
  riot_module_depends(stdin stdio_uart_rx)
endif()

add_subdirectory(core)
add_subdirectory(cpu)
add_subdirectory(boards)
add_subdirectory(drivers)
add_subdirectory(sys)
