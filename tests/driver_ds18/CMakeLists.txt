add_executable(test_driver_ds18 main.c)
target_link_libraries(test_driver_ds18 PUBLIC RIOT::RIOT)
riot_target_depends(test_driver_ds18 PUBLIC
    ds18
    xtimer printf_float
    # Use the module if you have an accurate sleep with xtimer (~3us)
    #ds18_optimized
)
riot_check_missing_modules(test_driver_ds18)
