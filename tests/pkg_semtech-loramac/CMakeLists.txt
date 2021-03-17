add_executable(test_pkg_semtech-loramac main.c)
target_link_libraries(test_pkg_semtech-loramac PUBLIC RIOT::RIOT)
set(RIOT_TEST_PKG_SEMTECH-LORAMAC_DRIVER "sx1276" CACHE STRING "The LORA driver used in test_pkg_semtech-loramac")
set(RIOT_TEST_PKG_SEMTECH-LORAMAC_REGION "EU868" CACHE STRING "The LORA radio region used in test_pkg_semtech-loramac")
mark_as_advanced(RIOT_TEST_PKG_SEMTECH-LORAMAC_DRIVER)
mark_as_advanced(RIOT_TEST_PKG_SEMTECH-LORAMAC_REGION)
riot_target_depends(test_pkg_semtech-loramac PUBLIC ${RIOT_TEST_PKG_SEMTECH-LORAMAC_DRIVER} auto_init_loramac shell shell_commands fmt)
set(boards_without_lora_rx
    arduino-mega2560
    i-nucleo-lrwan1
    stm32f0discovery
    waspmote-pro
)
if (NOT BOARD IN_LIST boards_without_lora_rx)
  riot_target_depends(test_pkg_semtech-loramac PUBLIC semtech_loramac_rx)
endif()
riot_check_missing_modules(test_pkg_semtech-loramac)
