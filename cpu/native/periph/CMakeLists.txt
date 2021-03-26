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

riot_module_depends(periph_rtc xtimer)
add_riot_module(periph_flashpage_pagewise)
riot_module_depends(periph_flashpage_pagewise periph_flashpage)
