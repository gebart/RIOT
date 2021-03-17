add_executable(test_blob main.c)
target_link_libraries(test_blob PUBLIC RIOT::RIOT)
riot_target_depends(test_blob PUBLIC fmt blob_module)
riot_check_missing_modules(test_blob)
