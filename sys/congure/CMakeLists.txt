add_riot_module(congure INTERFACE)
add_riot_module(congure_mock OBJECT mock/congure_mock.c)
riot_module_depends(congure_mock congure)
add_riot_module(congure_test OBJECT test/congure_test.c)
riot_module_depends(congure_test congure fmt)

set(CONGURE_TEST_LOST_MSG_POOL_SIZE 4 CACHE STRING "Pool size for the list elements for a lost message report")
target_compile_definitions(riot_module_congure_test PUBLIC CONGURE_TEST_LOST_MSG_POOL_SIZE=${CONGURE_TEST_LOST_MSG_POOL_SIZE})
