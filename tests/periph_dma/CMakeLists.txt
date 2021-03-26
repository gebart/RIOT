add_executable(test_periph_dma main.c)
target_link_libraries(test_periph_dma PUBLIC RIOT::RIOT)
riot_target_depends(test_periph_dma PUBLIC periph_dma)
riot_check_missing_modules(test_periph_dma)
