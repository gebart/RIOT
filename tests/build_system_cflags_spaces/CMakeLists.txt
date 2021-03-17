add_executable(test_build_system_cflags_spaces main.c)
target_link_libraries(test_build_system_cflags_spaces PUBLIC RIOT::RIOT)
target_compile_definitions(test_build_system_cflags_spaces PRIVATE
    "SUPER_STRING=\"I love sentences with spaces\""
    "DEFINED_AFTER_MAKEFILE_INCLUDE=0"
    )
riot_check_missing_modules(test_build_system_cflags_spaces)
