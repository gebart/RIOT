include_guard(GLOBAL)
# Enter all immediate subdirectories which have a CMakeLists.txt file
macro(add_all_subdirectories)
  file(GLOB subdirectory_cmakelists */CMakeLists.txt)
  foreach (cmakelist IN LISTS subdirectory_cmakelists)
    get_filename_component(subdir "${cmakelist}" DIRECTORY)
    add_subdirectory(${subdir})
  endforeach()
endmacro()

# Create a RIOT module library target and set up the expected MODULE_x compiler defintion
function(add_riot_module module_name)
  string(TOUPPER "${module_name}" module_name_u)
  string(MAKE_C_IDENTIFIER "${module_name_u}" module_name_c)
  set(prefixed_name "riot_module_${module_name}")
  message(VERBOSE "Creating RIOT module target ${prefixed_name}")
  add_library(${prefixed_name} INTERFACE)
  if (ARGN)
    target_sources(${prefixed_name} INTERFACE ${ARGN})
  endif()
  target_compile_definitions(${prefixed_name} INTERFACE MODULE_${module_name_c})
  if (TARGET riot_revdeps_${module_name})
    get_target_property(revdeps riot_revdeps_${module_name} INTERFACE_RIOT_REVDEPS)
    if (revdeps)
      foreach (revdep IN LISTS revdeps)
        get_target_property(missing_mods ${revdep} INTERFACE_RIOT_MISSING_MODULES)
        list(REMOVE_ITEM missing_mods ${module_name})
        set_property(TARGET ${revdep} PROPERTY INTERFACE_RIOT_MISSING_MODULES ${missing_mods})
      endforeach()
    endif()
  endif()
endfunction()

# Check if any RIOT modules have not been resolved.
# This function is a convenience helper to exclude test cases which depend on features that are not implemented for the chosen BOARD.
function(riot_check_missing_modules target_name)
  get_target_property(missing_mods ${target_name} INTERFACE_RIOT_MISSING_MODULES)
  if (missing_mods)
    message(VERBOSE "Excluding ${target_name} because of missing dependencies: ${missing_mods}")
    set_property(GLOBAL APPEND PROPERTY RIOT_MISSING_MODULE_TARGETS ${target_name})
  endif()
endfunction()

# This replaces the USEMODULE += module pattern found in the old Makefile based build system
#function(riot_use_module module_name) # , module_name2, module_name3, ... (variadic)
#  message(VERBOSE "Using RIOT modules: ${ARGV}")
#  riot_internal_module_target_depends(riot_modules riot_modules riot_modules_flags ${ARGV})
#endfunction()

# Specifying dependencies on RIOT modules for any CMake target
function(riot_target_depends target_name link_kind) # , [dependencies]
  message(DEBUG "Adding dependency: ${target_name} -> ${ARGN}")
  riot_internal_propagate_target_properties(${target_name} ${ARGN})
  foreach (module_name IN LISTS ARGN)
    riot_module_target_name_unless_disabled_genex(dependency_name_conditional ${module_name})
    target_link_libraries(${target_name} ${link_kind} ${dependency_name_conditional})
  endforeach()
endfunction()

# Specifying dependencies between modules
function(riot_module_depends module_name) # , [dependencies]
  riot_target_depends(riot_module_${module_name} INTERFACE ${ARGN})
endfunction()

# Depend on a module only if it exists at generation time
# TODO Set USEMODULE for transitive deps of optional deps
function(riot_target_depends_optional target_name link_kind) # , [dependencies]
  message(VERBOSE "Adding optional dependency: ${target_name} -> ${ARGN}")
  foreach (dependency IN LISTS ARGN)
    target_link_libraries(${target_name} ${link_kind} $<TARGET_NAME_IF_EXISTS:riot_module_${dependency}>)
  endforeach()
endfunction()

# Specifying optional dependencies between modules
function(riot_module_depends_optional module_name)
  riot_target_depends_optional(riot_module_${module_name} INTERFACE ${ARGN})
endfunction()

# Depend on a module if a generator expression condition is satisfied
#function(riot_module_depends_if module_name genex_condition) # , [dependencies]
#  message(VERBOSE "Adding conditional dependency: ${module_name} -> ${ARGN}")
#  target_link_libraries(riot_module_${module_name} INTERFACE $<$<BOOL:${genex_condition}>:${ARGN}>)
#endfunction()

function(riot_target_module_disable target_name module_name)
  string(MAKE_C_IDENTIFIER "${module_name}" module_name_id)
  set_property(TARGET ${target_name} PROPERTY INTERFACE_RIOT_DISABLE_MODULE_${module_name_id} 1)
endfunction()

function(riot_module_target_name_unless_disabled_genex out_var module_name)
  string(MAKE_C_IDENTIFIER "${module_name}" module_name_id)
  set(${out_var} "$<$<NOT:$<BOOL:$<TARGET_PROPERTY:INTERFACE_RIOT_DISABLE_MODULE_${module_name_id}>>>:riot_module_${module_name}>" PARENT_SCOPE)
endfunction()

# Internal helper method to avoid duplicating the configuration steps
function(riot_internal_propagate_target_properties target_name) # , [dependencies]
  set(dependencies ${ARGN})
  set(revdeps "")
  list(APPEND CMAKE_MESSAGE_INDENT "  ")
  if (target_name MATCHES "^riot_module_(.*)$")
    set(module_name ${CMAKE_MATCH_1})
    if (TARGET riot_revdeps_${module_name})
      get_target_property(revdeps riot_revdeps_${module_name} INTERFACE_RIOT_REVDEPS)
      if (NOT revdeps)
        # avoid "revdeps-NOT_FOUND"
        set(revdeps "")
      endif()
    endif()
    message(DEBUG "module_name: ${module_name}, revdeps: ${revdeps}")
  else()
    get_target_property(deps ${target_name} INTERFACE_RIOT_USEMODULE)
    if (NOT deps)
      # If we are adding dependencies to a previously unused target, then we
      # must also bring in the core system dependencies or some USEMODULE
      # checks will fail and we will not get the flags that we expect.
      list(PREPEND dependencies RIOT)
      get_target_property(exclude_from_all ${target_name} EXCLUDE_FROM_ALL)
      if (NOT exclude_from_all)
        # Exclude from all if not all dependencies are satisfied
        set_property(TARGET ${target_name} PROPERTY EXCLUDE_FROM_ALL $<BOOL:$<TARGET_PROPERTY:INTERFACE_RIOT_MISSING_MODULES>>)
      endif()
    endif()
  endif()
  list(REMOVE_DUPLICATES dependencies)
  list(REMOVE_DUPLICATES revdeps)
  foreach (dependency IN LISTS dependencies)
    message(DEBUG "${target_name} -> ${dependency}")
    string(MAKE_C_IDENTIFIER "${dependency}" dependency_id)
#    target_sources(${module_target} INTERFACE $<$<STREQUAL:$<TARGET_PROPERTY:riot_module_${dependency},TYPE>,OBJECT_LIBRARY>:$<TARGET_OBJECTS:riot_module_${dependency}>>)
    set_property(TARGET ${target_name} ${revdeps} APPEND PROPERTY INTERFACE_RIOT_USEMODULE ${dependency})
    set_property(TARGET ${target_name} ${revdeps} PROPERTY INTERFACE_RIOT_MODULE_${dependency_id} 1)
    if (NOT TARGET riot_revdeps_${dependency})
      message(DEBUG "Creating revdeps target: riot_revdeps_${dependency}")
      add_library(riot_revdeps_${dependency} INTERFACE)
    endif()
    set_property(TARGET riot_revdeps_${dependency} APPEND PROPERTY INTERFACE_RIOT_REVDEPS ${target_name} ${revdeps})
    if (TARGET riot_module_${dependency})
      message(VERBOSE "propagating transitive dependencies of ${dependency} to ${target_name} ${revdeps}")
      get_target_property(missing_mods riot_module_${dependency} INTERFACE_RIOT_MISSING_MODULES)
      if (missing_mods)
        set_property(TARGET ${target_name} ${revdeps} APPEND PROPERTY INTERFACE_RIOT_MISSING_MODULES ${missing_mods})
      endif()
      get_target_property(usemodules riot_module_${dependency} INTERFACE_RIOT_USEMODULE)
      message(DEBUG "usemodules: ${usemodules}")
      if (usemodules)
        list(REMOVE_DUPLICATES usemodules)
        set_property(TARGET ${target_name} ${revdeps} APPEND PROPERTY INTERFACE_RIOT_USEMODULE ${usemodules})
        get_target_property(debugdeps ${target_name} INTERFACE_RIOT_USEMODULE)
        message(DEBUG "debugdeps: ${debugdeps}")
        foreach (transdep IN LISTS usemodules)
          string(MAKE_C_IDENTIFIER "${transdep}" transdep_id)
          message(DEBUG "transdep: ${transdep_id}")
          # riot_revdeps_${transdep} has already been created when the first direct dependency was added
          set_property(TARGET riot_revdeps_${transdep} APPEND PROPERTY INTERFACE_RIOT_REVDEPS ${target_name} ${revdeps})
          set_property(TARGET ${target_name} ${revdeps} PROPERTY INTERFACE_RIOT_MODULE_${transdep_id} 1)
        endforeach()
      endif()
    else()
      message(VERBOSE "Module ${dependency} not yet defined")
      set_property(TARGET ${target_name} ${revdeps} APPEND PROPERTY INTERFACE_RIOT_MISSING_MODULES ${dependency})
    endif()
  endforeach()
  list(POP_BACK CMAKE_MESSAGE_INDENT)
endfunction()