add_executable(test_cpp_ctors main.c tests-cpp_ctors.cpp tests-cpp_ctors-class.cpp)
target_link_libraries(test_cpp_ctors PUBLIC RIOT::RIOT)
riot_target_depends(test_cpp_ctors PUBLIC embunit)
riot_check_missing_modules(test_cpp_ctors)
