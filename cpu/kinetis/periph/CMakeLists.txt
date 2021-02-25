set(submodules adc dac flashpage gpio hwrng i2c ics mcg mcg_lite pm pwm rtc rtt spi timer uart wdog)
foreach(submodule IN LISTS submodules)
  add_riot_module(periph_${submodule} OBJECT ${submodule}.c)
  riot_module_depends(periph_${submodule} periph_common)
endforeach()
riot_use_module(periph_gpio)

riot_module_depends(periph_i2c core_thread_flags)
riot_module_depends(periph_rtc periph_rtt)
