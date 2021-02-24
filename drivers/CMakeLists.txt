add_library(riot_drivers_flags INTERFACE)
target_include_directories(riot_drivers_flags INTERFACE include)
target_link_libraries(riot_flags INTERFACE riot_drivers_flags)

# Enter all immediate subdirectories which have a CMakeLists.txt file
file(GLOB cmakelists */CMakeLists.txt)
foreach (cmakelist IN LISTS cmakelists)
  get_filename_component(subdir "${cmakelist}" DIRECTORY)
  add_subdirectory(${subdir})
endforeach()
