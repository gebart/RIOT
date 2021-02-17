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
set(CMAKE_C_EXTENSIONS OFF)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_library(riot_flags INTERFACE)

if (NOT BOARD STREQUAL native OR NOT CMAKE_SYSTEM_NAME STREQUAL Darwin)
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
  if (CMAKE_C_COMPILER_ID STREQUAL GNU)
    target_compile_options(riot_flags INTERFACE -Wformat-truncation -Wformat-signedness)
  endif ()
  #add_compile_options($<$<COMPILE_LANGUAGE:CXX>:-Woverloaded-virtual>)
  #add_compile_options($<$<COMPILE_LANGUAGE:CXX>:-Weffc++>)
endif ()

add_library(RIOT INTERFACE)
add_subdirectory(cpu)
add_subdirectory(boards)
add_subdirectory(core)
target_link_libraries(RIOT INTERFACE
  riot_cpu
  riot_board
  riot_core
  )

add_subdirectory(drivers)
add_subdirectory(sys)

if (CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME AND BUILD_TESTING)
  add_subdirectory(tests)
endif ()
