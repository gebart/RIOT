add_library(riot_sys_flags INTERFACE)
target_include_directories(riot_sys_flags INTERFACE include)
target_link_libraries(riot_flags INTERFACE riot_sys_flags)

add_all_subdirectories()

add_riot_module(newlib INTERFACE)
target_link_libraries(riot_module_newlib INTERFACE c)

add_riot_module(newlib_nano INTERFACE)
target_compile_options(riot_module_newlib_nano INTERFACE -specs=nano.specs)
target_link_options(riot_module_newlib_nano INTERFACE -specs=nano.specs)

# Test for Newlib Nano (nano.specs)
include(CheckIncludeFile)
message(CHECK_START "Checking for newlib-nano")
check_include_file(newlib.h HAVE_NEWLIB_NANO -specs=nano.specs)
if (HAVE_NEWLIB_NANO)
  message(CHECK_PASS "found")
  riot_module_depends(newlib newlib_nano)
else()
  message(CHECK_FAIL "not found")
endif()

add_riot_module(newlib_gnu_source INTERFACE)
target_compile_definitions(riot_module_newlib_gnu_source INTERFACE _GNU_SOURCE)

