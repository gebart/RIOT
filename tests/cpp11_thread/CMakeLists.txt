add_executable(test_cpp11_thread main.cpp)
target_link_libraries(test_cpp11_thread PUBLIC RIOT::RIOT)
riot_target_depends(test_cpp11_thread PUBLIC cpp11-compat xtimer timex)
riot_check_missing_modules(test_cpp11_thread)
