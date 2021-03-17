add_executable(test_pkg_cmsis-dsp main.c)
target_link_libraries(test_pkg_cmsis-dsp PUBLIC RIOT::RIOT)
riot_target_depends(test_pkg_cmsis-dsp PUBLIC )
riot_check_missing_modules(test_pkg_cmsis-dsp)
