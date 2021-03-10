add_executable(test_thread_race main.c)
target_link_libraries(test_thread_race PUBLIC RIOT::RIOT)

# stm32f030f4-demo doesn't have enough RAM to run the test
# so we reduce the stack size for every thread
if (BOARD STREQUAL "stm32f030f4-demo")
  target_compile_definitions(test_thread_race THREAD_STACKSIZE_DEFAULT=512)
endif()
