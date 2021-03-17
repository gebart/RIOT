add_executable(test_puf_sram main.c)
target_link_libraries(test_puf_sram PUBLIC RIOT::RIOT)
riot_target_depends(test_puf_sram PUBLIC )
riot_check_missing_modules(test_puf_sram)
