add_executable(test_rmutex_cpp main.cpp)
target_link_libraries(test_rmutex_cpp PUBLIC RIOT::RIOT)
riot_target_depends(test_rmutex_cpp PUBLIC )
riot_check_missing_modules(test_rmutex_cpp)
