add_executable(test_xtimer_periodic_wakeup main.c)
target_link_libraries(test_xtimer_periodic_wakeup PUBLIC RIOT::RIOT)
riot_target_depends(test_xtimer_periodic_wakeup PUBLIC xtimer)

# stm32f030f4-demo doesn't have enough RAM to run the test
# so we reduce the stack size for every thread
if (BOARD STREQUAL "stm32f030f4-demo")
  target_compile_definitions(test_xtimer_periodic_wakeup THREAD_STACKSIZE_DEFAULT=512)
endif()
