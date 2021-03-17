add_executable(test_driver_lis2dh12 main.c)
target_link_libraries(test_driver_lis2dh12 PUBLIC RIOT::RIOT)
# use lis2dh12_spi for SPI-Mode and lis2dh12_i2c for I2C-Mode
set(RIOT_TEST_DRIVER_LIS2DH12_MODEL "lis2dh12_spi" CACHE STRING "The specific device to build test_driver_lis2dh12 for")
mark_as_advanced(RIOT_TEST_DRIVER_LIS2DH12_MODEL)
riot_target_depends(test_driver_lis2dh12 PUBLIC fmt xtimer ${RIOT_TEST_DRIVER_LIS2DH12_MODEL} lis2dh12_int)
riot_check_missing_modules(test_driver_lis2dh12)
