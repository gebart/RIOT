add_executable(test_cpp11_mutex main.cpp)
target_link_libraries(test_cpp11_mutex PUBLIC RIOT::RIOT)
riot_target_depends(test_cpp11_mutex PUBLIC cpp11-compat xtimer)
riot_check_missing_modules(test_cpp11_mutex)
