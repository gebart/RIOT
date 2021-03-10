add_riot_module(fxos8700
  fxos8700.c
  fxos8700_saul.c
)
target_include_directories(riot_module_fxos8700 INTERFACE include)
riot_module_depends(fxos8700 xtimer periph_i2c)
