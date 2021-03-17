add_executable(test_socket_zep main.c)
target_link_libraries(test_socket_zep PUBLIC RIOT::RIOT)
riot_target_depends(test_socket_zep PUBLIC od socket_zep)
riot_check_missing_modules(test_socket_zep)
