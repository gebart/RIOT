add_executable(test_pkg_wolfssl main.c)
target_link_libraries(test_pkg_wolfssl PUBLIC RIOT::RIOT)

riot_target_depends(test_pkg_wolfssl PUBLIC
    wolfssl
    wolfcrypt
    wolfcrypt-test
    wolfcrypt_sha512
    wolfcrypt_curve25519
    wolfcrypt_ed25519
    wolfcrypt_chacha
    wolfcrypt_poly1305
    wolfcrypt_aes
    wolfcrypt_ecc
    wolfcrypt_asn
    wolfcrypt_random

    # Uncomment the following line to enable RSA tests
    # (e.g. when enough resources are available on platform)
    #wolfcrypt_rsa wolfcrypt_dh

    # Comment the following line to disable full-benchmark test
    wolfcrypt-benchmark
    xtimer
)

# This is an optimized stack value based on testing, if you observe
# a segmentation fault please increase this stack size.
target_compile_definitions(test_pkg_wolfssl PUBLIC "THREAD_STACKSIZE_MAIN=2*THREAD_STACKSIZE_LARGE")

if (NOT BOARD STREQUAL "native")
  target_compile_definitions(test_pkg_wolfssl PRIVATE BENCH_EMBEDDED)
endif()

riot_check_missing_modules(test_pkg_wolfssl)
