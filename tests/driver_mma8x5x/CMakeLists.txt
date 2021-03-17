add_executable(test_driver_mma8x5x main.c)
target_link_libraries(test_driver_mma8x5x PUBLIC RIOT::RIOT)
riot_target_depends(test_driver_mma8x5x PUBLIC
    mma8x5x # will work with all variants
    xtimer
)
riot_check_missing_modules(test_driver_mma8x5x)
