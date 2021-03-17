add_riot_module(test_rng main.c)

# Default stack with printf + the tests buffer uint32_t[32]/uint8_t[256]
target_compile_definitions(riot_module_test_rng INTERFACE "THREAD_STACKSIZE_MAIN=(THREAD_STACKSIZE_DEFAULT+THREAD_EXTRA_STACKSIZE_PRINTF+256)")
riot_module_depends(test_rng fmt random shell xtimer)

# TODO Glob prngs from available modules
set(prngs
    fortuna
    hwrng
    mersenne
    minstd
    musl_lcg
    sha1prng
    sha256prng
    tinymt32
    xorshift
    )
foreach(rng IN LISTS prngs)
  add_executable(test_rng_${rng})
  target_link_libraries(test_rng_${rng} PUBLIC RIOT::RIOT)
  riot_target_depends(test_rng_${rng} PUBLIC test_rng prng_${rng})
  riot_check_missing_modules(test_rng_${rng})
endforeach()
