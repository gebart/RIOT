add_executable(test_periph_qdec main.c)
target_link_libraries(test_periph_qdec PUBLIC RIOT::RIOT)
riot_target_depends(test_periph_qdec PUBLIC periph_qdec xtimer)
riot_check_missing_modules(test_periph_qdec)
