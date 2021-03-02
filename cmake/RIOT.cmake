## Path configuration
get_filename_component(RIOT_BASEDIR "${CMAKE_CURRENT_LIST_DIR}" DIRECTORY CACHE)
set(RIOT_BASEDIR "${RIOT_BASEDIR}" CACHE PATH "Path to the RIOT source tree root")
list(APPEND CMAKE_MODULE_PATH "${RIOT_BASEDIR}/cmake")

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
set(RIOT_BOARD_DIR "${RIOT_BASEDIR}/boards/${BOARD}" CACHE PATH "Path to the board-specific code")
list(APPEND CMAKE_MODULE_PATH "${RIOT_BOARD_DIR}/cmake")
include(riot_board)

# riot_board.cmake above should set ${CPU}
set(RIOT_CPU_DIR "${RIOT_BASEDIR}/cpu/${CPU}" CACHE PATH "Path to the CPU-specific code")
list(APPEND CMAKE_MODULE_PATH "${RIOT_CPU_DIR}/cmake")
include(riot_cpu)

add_subdirectory("${RIOT_BASEDIR}" "${CMAKE_CURRENT_BINARY_DIR}/RIOT")

