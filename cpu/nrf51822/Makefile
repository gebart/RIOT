# define the module that is build
MODULE = cpu

# add a list of subdirectories, that should also be build
DIRS = periph $(CORTEX_COMMON)

# also build the radio driver when enabled
ifneq (,$(filter nrf51prop,$(USEMODULE)))
    DIRS += nrf51prop
endif

include $(RIOTBASE)/Makefile.base
