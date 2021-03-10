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
endfunction()

# This replaces the USEMODULE += module pattern found in the old Makefile based build system
#function(riot_use_module module_name) # , module_name2, module_name3, ... (variadic)
#  message(VERBOSE "Using RIOT modules: ${ARGV}")
#  riot_internal_module_target_depends(riot_modules riot_modules riot_modules_flags ${ARGV})
#endfunction()

# Specifying dependencies on RIOT modules for any CMake target
function(riot_target_depends target_name link_kind) # , [dependencies]
  message(DEBUG "Adding dependency: ${target_name} -> ${ARGN}")
  list(TRANSFORM ARGN PREPEND "riot_module_")
  target_link_libraries(${target_name} ${link_kind} ${ARGN})
endfunction()

# Specifying dependencies between modules
function(riot_module_depends module_name) # , [dependencies]
  riot_target_depends(riot_module_${module_name} INTERFACE ${ARGN})
endfunction()

# Depend on a module only if it exists at generation time
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

## Internal helper method to avoid duplicating the configuration steps
#function(riot_internal_module_target_depends module_name module_target module_flags_target) # , [dependencies]
#  set(dependencies_targets ${ARGN})
#  list(TRANSFORM dependencies_targets PREPEND "riot_module_")
#  target_link_libraries(${module_target} INTERFACE ${dependencies_targets})
#  set(dependencies_module_flags_targets ${dependencies_targets})
#  list(TRANSFORM dependencies_module_flags_targets APPEND "_module_flags")
#  target_link_libraries(${module_flags_target} INTERFACE ${dependencies_module_flags_targets})
#  list(APPEND CMAKE_MESSAGE_INDENT "  ")
#  string(MAKE_C_IDENTIFIER "${module_name}" module_name_id)
#  get_target_property(revdeps riot_modules INTERFACE_RIOT_REVDEPS_${module_name_id})
#  if (NOT revdeps)
#    # avoid revdeps-NOT_FOUND
#    set(revdeps "")
#  endif()
#  list(REMOVE_DUPLICATES revdeps)
#  foreach (dependency IN LISTS ARGN)
#    message(DEBUG "${module_target} -> ${dependency}")
#    string(MAKE_C_IDENTIFIER "${dependency}" dependency_id)
#    target_sources(${module_target} INTERFACE $<$<STREQUAL:$<TARGET_PROPERTY:riot_module_${dependency},TYPE>,OBJECT_LIBRARY>:$<TARGET_OBJECTS:riot_module_${dependency}>>)
#    set_property(TARGET ${module_target} ${revdeps} APPEND PROPERTY INTERFACE_RIOT_USEMODULE ${dependency})
#    set_property(TARGET ${module_target} ${revdeps} PROPERTY INTERFACE_RIOT_MODULE_${dependency_id} 1)
#    set_property(TARGET riot_modules APPEND PROPERTY INTERFACE_RIOT_REVDEPS_${dependency_id} ${module_target} ${revdeps})
#    if (TARGET riot_module_${dependency})
#      message(VERBOSE "propagating transitive dependencies of ${dependency} to ${module_name} ${revdeps}")
#      get_target_property(deps riot_module_${dependency} INTERFACE_RIOT_USEMODULE)
#      if (deps)
#        list(REMOVE_DUPLICATES deps)
#        set_property(TARGET ${module_target} ${revdeps} APPEND PROPERTY INTERFACE_RIOT_USEMODULE ${dependency})
#        foreach (transdep IN LISTS deps)
#          string(MAKE_C_IDENTIFIER "${transdep}" transdep_id)
#          set_property(TARGET ${module_target} ${revdeps} PROPERTY INTERFACE_RIOT_MODULE_${transdep_id} 1)
#        endforeach()
#      endif()
#    endif()
#    foreach (dependent IN LISTS revdeps)
#      message(VERBOSE "propagating transitive dependency ${dependency} of ${module_name} to ${dependent}")
#    endforeach()
#  endforeach()
#  list(POP_BACK CMAKE_MESSAGE_INDENT)
#endfunction()
