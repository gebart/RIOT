set(submodules
    can
    cpuid
    eeprom
    flashpage
    gpio_linux
    gpio_mock
    hwrng
    pm
    pwm
    qdec
    rtc
    spidev_linux
    timer
    uart
)
foreach(submodule IN LISTS submodules)
  add_riot_module(periph_${submodule} ${submodule}.c)
  riot_module_depends(periph_${submodule} periph_common)
endforeach()
