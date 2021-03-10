add_riot_module(sys)
target_include_directories(riot_module_sys INTERFACE include)
riot_module_depends(RIOT sys)

add_all_subdirectories()

add_riot_module(newlib)
target_link_libraries(riot_module_newlib INTERFACE c)

add_riot_module(newlib_nano)
target_compile_options(riot_module_newlib_nano INTERFACE -specs=nano.specs)
target_link_options(riot_module_newlib_nano INTERFACE -specs=nano.specs)

# Test for Newlib Nano (nano.specs)
include(CheckIncludeFile)
message(CHECK_START "Checking for newlib-nano")
list(APPEND CMAKE_MESSAGE_INDENT "  ")
check_include_file(newlib.h HAVE_NEWLIB_NANO -specs=nano.specs)
list(POP_BACK CMAKE_MESSAGE_INDENT)
if (HAVE_NEWLIB_NANO)
  message(CHECK_PASS "found")
  riot_module_depends(newlib newlib_nano)
else()
  message(CHECK_FAIL "not found")
endif()

add_riot_module(newlib_gnu_source)
target_compile_definitions(riot_module_newlib_gnu_source INTERFACE _GNU_SOURCE)

add_riot_module(printf_float)
target_link_options(riot_module_printf_float INTERFACE $<$<BOOL:$<TARGET_PROPERTY:INTERFACE_RIOT_MODULE_newlib_nano>>:-u _printf_float>)
