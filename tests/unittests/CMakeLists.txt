add_executable(test_unittests main.c)
target_link_libraries(test_unittests PUBLIC RIOT::RIOT)
riot_target_depends(test_unittests PUBLIC embunit)

riot_target_module_disable(test_unittests auto_init)
# some tests need more stack
target_compile_definitions(test_unittests PUBLIC "THREAD_STACKSIZE_MAIN=THREAD_STACKSIZE_LARGE")

riot_check_missing_modules(test_unittests)
