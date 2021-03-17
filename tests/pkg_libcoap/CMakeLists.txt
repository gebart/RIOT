add_executable(test_pkg_libcoap main.c)
target_link_libraries(test_pkg_libcoap PUBLIC RIOT::RIOT)
riot_target_depends(test_pkg_libcoap PUBLIC gnrc_ipv6 sock_udp)
riot_check_missing_modules(test_pkg_libcoap)
