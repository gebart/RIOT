set(submodules adc dac flashpage gpio hwrng i2c ics mcg mcg_lite pm pwm rtc rtt spi timer uart wdog)
foreach(submodule IN LISTS submodules)
  add_riot_module(periph_${submodule} ${submodule}.c)
  riot_module_depends(periph_${submodule} periph_common)
endforeach()
riot_module_depends(cpu periph_gpio)

riot_module_depends(periph_adc periph_gpio)
riot_module_depends(periph_i2c periph_gpio core_thread_flags)
riot_module_depends(periph_pwm periph_gpio)
riot_module_depends(periph_spi periph_gpio)
riot_module_depends(periph_rtc periph_rtt)
riot_module_depends(periph_uart periph_gpio)

# cpuid is provided by periph_common via the CPUID_ADDR define in periph_cpu.h
add_riot_module(periph_cpuid)
riot_module_depends(periph_cpuid periph_common)
# periph_gpio_irq is provided by periph_gpio
add_riot_module(periph_gpio_irq)
riot_module_depends(periph_gpio_irq periph_gpio)
