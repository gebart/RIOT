add_executable(test_cpp11_condition_variable main.cpp)
target_link_libraries(test_cpp11_condition_variable PUBLIC RIOT::RIOT)
riot_target_depends(test_cpp11_condition_variable PUBLIC cpp11-compat xtimer timex)

# nucleo-f303k8 doesn't have enough RAM to run the test so we reduce the stack
# size for every thread
set(small_ram_boards nucleo-f303k8 nucleo-f334r8)
if (BOARD IN_LIST small_ram_boards)
  target_compile_definitions(test_cpp11_condition_variable PUBLIC THREAD_STACKSIZE_DEFAULT=512)
endif()

riot_check_missing_modules(test_cpp11_condition_variable)
