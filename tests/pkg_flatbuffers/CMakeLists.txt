add_executable(test_pkg_flatbuffers main.cpp)
target_link_libraries(test_pkg_flatbuffers PUBLIC RIOT::RIOT)
riot_target_depends(test_pkg_flatbuffers PUBLIC flatbuffers)
if (CXX_COMPILER_ID STREQUAL "GNU" OR CXX_COMPILER_ID MATCHES "Clang")
  # Skip warning raised when building monster_generated.h
  target_compile_options(test_pkg_flatbuffers PUBLIC -Wno-type-limits)
endif()
riot_check_missing_modules(test_pkg_flatbuffers)
