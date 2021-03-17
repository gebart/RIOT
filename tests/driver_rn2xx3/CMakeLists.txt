add_executable(test_driver_rn2xx3 main.c)
target_link_libraries(test_driver_rn2xx3 PUBLIC RIOT::RIOT)

set(RIOT_TEST_DRIVER_RN2XX3_MODEL "rn2483" CACHE STRING "The specific device to build test_driver_rn2xx3 for")
mark_as_advanced(RIOT_TEST_DRIVER_RN2XX3_MODEL)
riot_target_depends(test_driver_rn2xx3 PUBLIC ${RIOT_TEST_DRIVER_RN2XX3_MODEL} shell shell_commands)
riot_check_missing_modules(test_driver_rn2xx3)
