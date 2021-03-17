add_executable(test_cpp_ext main.cpp)
target_link_libraries(test_cpp_ext PUBLIC RIOT::RIOT)
riot_target_depends(test_cpp_ext PUBLIC test_cpp_ext_module)
riot_check_missing_modules(test_cpp_ext)
