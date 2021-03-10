add_executable(test_driver_fxos8700 main.c)
target_link_libraries(test_driver_fxos8700 PUBLIC RIOT::RIOT)
riot_target_depends(test_driver_fxos8700 PUBLIC fxos8700)
