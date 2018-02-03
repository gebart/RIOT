include ../Makefile.tests_common

# These boards only have a single timer in their periph_conf.h, needs special
# CFLAGS configuration to build properly
SINGLE_TIMER_BOARDS = \
  native \
  nucleo32-f031 \
  nucleo32-f042 \
  #

# These boards have too little RAM to support collecting detailed statistics
# with the default settings of TEST_MIN and TEST_MAX, so DETAILED_STATS will be
# disabled by default for these boards unless explicitly enabled
SMALL_RAM_BOARDS = \
  nucleo32-f031 \
  #

FEATURES_REQUIRED = periph_timer

USEMODULE += random
USEMODULE += fmt
USEMODULE += matstat

ifeq (,$(findstring TIM_TEST_DEV,$(CFLAGS)))
  ifneq (,$(filter $(BOARD),$(SINGLE_TIMER_BOARDS)))
    CFLAGS += -DTIM_TEST_DEV=TIMER_DEV\(0\)
  endif
endif

ifeq (,$(findstring DETAILED_STATS,$(CFLAGS)))
  ifneq (,$(filter $(BOARD),$(SMALL_RAM_BOARDS)))
    CFLAGS += -DDETAILED_STATS=0
  endif
endif

# Shortcut to configure the build for testing Kinetis LPTMR against a PIT reference
# Usage: make BOARD=frdm-k22f test-kinetis-lptmr flash
test-kinetis-lptmr: CFLAGS+=-DTIM_TEST_DEV=TIMER_LPTMR_DEV\(0\) -DTIM_TEST_FREQ=32768 -DTIM_REF_DEV=TIMER_PIT_DEV\(0\)
test-kinetis-lptmr: all

# Reset the default goal.
.DEFAULT_GOAL :=

include $(RIOTBASE)/Makefile.include
