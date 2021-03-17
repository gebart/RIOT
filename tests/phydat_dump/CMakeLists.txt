add_executable(test_phydat_dump main.c)
target_link_libraries(test_phydat_dump PUBLIC RIOT::RIOT)
riot_target_depends(test_phydat_dump PUBLIC phydat)
riot_check_missing_modules(test_phydat_dump)
