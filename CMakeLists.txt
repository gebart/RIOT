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

# This internal cache variable is required to pass the list of object library files to the top level CMakeLists
set(RIOT_STARTFILES "" CACHE INTERNAL "Linker dependencies that must be explicitly linked with any executable")

# Path configuration
set(RIOT_BASEDIR ${CMAKE_CURRENT_LIST_DIR} CACHE PATH "Path to the RIOT source tree root")
set(RIOT_CPU_BASEDIR ${RIOT_BASEDIR}/cpu CACHE PATH "Path to cpu component of the RIOT tree")
set(RIOT_BOARD_BASEDIR ${RIOT_BASEDIR}/board CACHE PATH "Path to board component of the RIOT tree")
set(RIOT_BOARD_DIR ${RIOT_BOARD_BASEDIR}/${BOARD} CACHE PATH "Path to board specific code")

option(RIOT_WERROR "Build with all warnings as errors" ON)

# Use riot_configure_target to set all required linker options and libraries for building an executable
macro(riot_configure_target name)
  target_link_libraries(${name} PRIVATE RIOT ${DERI_STARTFILES})
  if (LINKER_SCRIPT)
    set_target_properties(${name} PROPERTIES LINK_DEPENDS ${LINKER_SCRIPT})
  endif ()
  if (NOT BOARD STREQUAL native OR NOT CMAKE_SYSTEM_NAME STREQUAL Darwin)
    # Xcode ld64 does not support -Map
    target_link_options(${name} PRIVATE LINKER:-Map=$<TARGET_FILE_BASE_NAME:${name}>.map)
  endif ()
endmacro()

# Load board specific CMake settings
list(APPEND CMAKE_MODULE_PATH "${RIOT_BOARD_DIR}/cmake" "${RIOT_BASEDIR}/cmake")
include(riot_platform)
include(riot_common_options)

if (MSVC)
  add_compile_options(/W4)
  if (RIOT_WERROR)
    add_compile_options(/WX)
  endif ()
else ()
  # lots of warnings and all warnings as errors
  #-Wshadow -Wundef
  add_compile_options(-Wall -Wextra -pedantic)
  if (RIOT_WERROR)
    add_compile_options(-Werror)
  endif ()
  add_compile_options(-Wdouble-promotion -Wconversion)
  add_compile_options(-Wswitch-enum)
  add_compile_options(-Wuninitialized -Winit-self)
  add_compile_options(-Wformat=2)
  if (CMAKE_C_COMPILER_ID STREQUAL GNU)
    add_compile_options(-Wformat-truncation -Wformat-signedness)
  endif ()
  add_compile_options($<$<COMPILE_LANGUAGE:CXX>:-Woverloaded-virtual>)
  add_compile_options($<$<COMPILE_LANGUAGE:CXX>:-Weffc++>)
endif ()

add_subdirectory(cpu)
add_library(RIOT INTERFACE)
add_subdirectory(board)
add_subdirectory(core)
target_link_libraries(RIOT INTERFACE
  riot_cpu_${CPU}
  riot_board_${BOARD}
  riot_core
  )

add_subdirectory(drivers)
add_subdirectory(sys)

if (CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME AND BUILD_TESTING)
  add_subdirectory(tests)
endif ()
