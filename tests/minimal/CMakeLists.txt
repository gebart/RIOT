add_executable(test_minimal main.c)
target_link_libraries(test_minimal PUBLIC RIOT::RIOT)
#riot_target_depends(test_minimal PUBLIC stdio_null)

target_compile_definitions(test_minimal PUBLIC "NDEBUG" "LOG_LEVEL=LOG_NONE")

# adjust stack sizes to very small values
if (BOARD STREQUAL "native")
  target_compile_definitions(test_minimal PUBLIC "THREAD_STACKSIZE_MAIN=256" "THREAD_STACKSIZE_IDLE=128")
endif()

riot_check_missing_modules(test_minimal)
