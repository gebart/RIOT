add_executable(test_bloom_bytes main.c)
target_link_libraries(test_bloom_bytes PUBLIC RIOT::RIOT)
riot_target_depends(test_bloom_bytes PUBLIC hashes bloom random xtimer fmt)
riot_check_missing_modules(test_bloom_bytes)
