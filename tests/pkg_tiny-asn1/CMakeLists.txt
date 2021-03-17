add_executable(test_pkg_tiny-asn1 main.c)
target_link_libraries(test_pkg_tiny-asn1 PUBLIC RIOT::RIOT)
riot_target_depends(test_pkg_tiny-asn1 PUBLIC )
riot_check_missing_modules(test_pkg_tiny-asn1)
