add_riot_module(random random.c)
add_riot_module(prng)
riot_module_depends(random luid prng)
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

set(RIOT_PRNG "tinymt32" CACHE STRING "The pseudo-random number generator to use for the build")
set_property(CACHE RIOT_PRNG PROPERTY STRINGS ${prngs})
riot_module_depends(prng prng_${RIOT_PRNG})
list(REMOVE_ITEM prngs sha1prng sha256prng) # the SHA generators share common code
add_riot_module(prng_shaxprng shaxprng.c)
foreach (name IN LISTS prngs)
  add_riot_module(prng_${name} ${name}.c)
endforeach()

add_riot_module(fortuna fortuna/fortuna.c)
riot_module_depends(prng_fortuna fortuna hashes crypto xtimer)

add_riot_module(tinymt32 tinymt32/tinymt32.c)
riot_module_depends(prng_tinymt32 tinymt32)

add_riot_module(prng_sha1prng)
riot_module_depends(prng_sha1prng prng_shaxprng)

add_riot_module(prng_sha256prng)
riot_module_depends(prng_sha256prng prng_shaxprng)

riot_module_depends(prng_shaxprng hashes)
riot_module_depends(prng_hwrng periph_hwrng)

# TODO check that only one prng is enabled
# TODO check shaxprng is not used without sha1prng or sha256prng
