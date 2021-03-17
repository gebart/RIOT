add_subdirectory(models)

add_executable(test_pkg_utensor main.cpp)
target_link_libraries(test_pkg_utensor PUBLIC RIOT::RIOT)
riot_target_depends(test_pkg_utensor PUBLIC utensor test_pkg_utensor_models)
riot_check_missing_modules(test_pkg_utensor)
