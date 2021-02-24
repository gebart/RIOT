## Path configuration
set(RIOT_BASEDIR ${CMAKE_CURRENT_LIST_DIR}/.. CACHE PATH "Path to the RIOT source tree root")
set(RIOT_CPU_BASEDIR ${RIOT_BASEDIR}/cpu CACHE PATH "Path to cpu component of the RIOT tree")
set(RIOT_BOARD_BASEDIR ${RIOT_BASEDIR}/boards CACHE PATH "Path to board component of the RIOT tree")
set(RIOT_BOARD_DIR ${RIOT_BOARD_BASEDIR}/${BOARD} CACHE PATH "Path to board specific code")
list(APPEND CMAKE_MODULE_PATH "${RIOT_BASEDIR}/cmake")

option(RIOT_WERROR "Build with all warnings as errors" ON)

# Use riot_configure_target to set all required linker options and libraries for building an executable
macro(riot_configure_target name)
  target_link_libraries(${name} PRIVATE RIOT ${RIOT_STARTFILES})
  if (LINKER_SCRIPT)
    set_target_properties(${name} PROPERTIES LINK_DEPENDS ${LINKER_SCRIPT})
  endif ()
  if (NOT BOARD STREQUAL "native" OR NOT CMAKE_SYSTEM_NAME STREQUAL "Darwin")
    # Xcode ld64 does not support -Map
    target_link_options(${name} PRIVATE LINKER:-Map=$<TARGET_FILE_BASE_NAME:${name}>.map)
  endif ()
endmacro()

if (NOT BOARD STREQUAL native)
  set(CMAKE_EXECUTABLE_SUFFIX .elf)
endif ()

# Load board specific CMake settings
list(APPEND CMAKE_MODULE_PATH "${RIOT_BOARD_DIR}/cmake")
include(riot_board)

set(RIOT_CPU_DIR ${RIOT_CPU_BASEDIR}/${CPU} CACHE PATH "Path to CPU specific code")
list(APPEND CMAKE_MODULE_PATH "${RIOT_CPU_DIR}/cmake")
include(riot_cpu)

add_subdirectory(${RIOT_BASEDIR} ${CMAKE_CURRENT_BINARY_DIR}/RIOT)

