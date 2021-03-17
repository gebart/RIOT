add_executable(test_pbkdf2 main.c)
target_link_libraries(test_pbkdf2 PUBLIC RIOT::RIOT)
riot_target_depends(test_pbkdf2 PUBLIC stdin hashes base64)
riot_check_missing_modules(test_pbkdf2)
