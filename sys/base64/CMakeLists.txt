add_riot_module(base64 OBJECT base64.c)
add_riot_module(base64url INTERFACE)
riot_module_depends(base64url base64)
