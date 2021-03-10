add_riot_module(congure)
add_riot_module(congure_mock mock/congure_mock.c)
riot_module_depends(congure_mock congure)
add_riot_module(congure_test test/congure_test.c)
riot_module_depends(congure_test congure fmt)

set(CONGURE_TEST_LOST_MSG_POOL_SIZE 4 CACHE STRING "Pool size for the list elements for a lost message report")
target_compile_definitions(riot_module_congure_test INTERFACE CONGURE_TEST_LOST_MSG_POOL_SIZE=${CONGURE_TEST_LOST_MSG_POOL_SIZE})
