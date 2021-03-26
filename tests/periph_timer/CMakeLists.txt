add_executable(test_periph_timer main.c)
target_link_libraries(test_periph_timer PUBLIC RIOT::RIOT)
riot_target_depends(test_periph_timer PUBLIC periph_timer)

set(boards_timer_500khz
    atxmega-a1u-xpro
)

set(boards_timer_250khz
    arduino-duemilanove
    arduino-leonardo
    arduino-mega2560
    arduino-uno
    atmega256rfr2-xpro
    atmega328p
)

set(boards_timer_32khz
    e180-zg120b-tb
    hifive1
    hifive1b
    ikea-tradfri
    frdm-kw41z
    phynode-kw41z
    usb-kw41z
    openlabs-kw41z-mini
    frdm-k64f
    frdm-k22f
    slstk3400a
    slstk3401a
    slstk3402a
    sltb001a
    slwstk6000b-slwrb4150a
    slwstk6000b-slwrb4162a
    slwstk6220a
    stk3600
    stk3700
)

set(boards_timer_coreclock
    cc2538dk
    openmote-b
    openmote-cc2538
    remote-reva
    remote-revb
    waspmote-pro
)

if (BOARD IN_LIST boards_timer_500khz)
  set(timer_speed 500000)
elseif(BOARD IN_LIST boards_timer_250khz)
  set(timer_speed 250000)
elseif(BOARD IN_LIST boards_timer_32khz)
  set(timer_speed 32768)
elseif(BOARD IN_LIST boards_timer_coreclock)
  set(timer_speed CLOCK_CORECLOCK)
else()
  set(timer_speed 1000000)
endif()

target_compile_definitions(test_periph_timer PUBLIC TIMER_SPEED=${timer_speed})
# TODO TIMER_SPEED
riot_check_missing_modules(test_periph_timer)
