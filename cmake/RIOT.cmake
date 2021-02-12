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
list(APPEND CMAKE_MODULE_PATH
  "${RIOT_BOARD_DIR}/cmake"
  "${RIOT_CPU_DIR}/cmake"
  "${RIOT_BASEDIR}/cmake")
include(riot_board)
include(riot_cpu)
include(riot_common_options)

