add_riot_module(hashes STATIC cmac.c hashes.c md5.c pbkdf2.c sha1.c sha2xx_common.c sha3.c sha224.c sha256.c)
riot_module_depends(hashes crypto)
