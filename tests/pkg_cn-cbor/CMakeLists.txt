add_executable(test_pkg_cn-cbor main.c)
target_link_libraries(test_pkg_cn-cbor PUBLIC RIOT::RIOT)
riot_target_depends(test_pkg_cn-cbor PUBLIC embunit fmt memarray)
riot_check_missing_modules(test_pkg_cn-cbor)
