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
