APPLICATION = driver_l3g4200d
include ../Makefile.tests_common

FEATURES_REQUIRED += periph_gpio

# Define default pin mappings for some boards:
ifneq (,$(filter iot-lab_M3,$(BOARD)))
  export TEST_L3G4200D_I2C ?= I2C_0
  export TEST_L3G4200D_ADDR ?= 104
  export TEST_L3G4200D_INT ?= GPIO_8
  export TEST_L3G4200D_DRDY ?= GPIO_9
endif
ifneq (,$(filter stm32f3discovery,$(BOARD)))
  export TEST_L3G4200D_SPI ?= SPI_0
  export TEST_L3G4200D_CS ?= 104
  export TEST_L3G4200D_INT ?= GPIO_8
  export TEST_L3G4200D_DRDY ?= GPIO_9
endif

USEMODULE += l3g4200d
USEMODULE += vtimer

# use I2C per default
ifneq (,$(TEST_L3G4200D_SPI))
  FEATURES_REQUIRED += periph_spi
  CFLAGS += -DTEST_L3G4200D_SPI=$(TEST_L3G4200D_SPI)
else
  FEATURES_REQUIRED += periph_i2c
  ifneq (,$(TEST_L3G4200D_I2C))
    CFLAGS += -DTEST_L3G4200D_I2C=$(TEST_L3G4200D_I2C)
  else
    # set random default
    CFLAGS += -DTEST_L3G4200D_I2C=I2C_0
  endif
endif
ifneq (,$(TEST_L3G4200D_CS))
  CFLAGS += -DTEST_L3G4200D_CS=$(TEST_L3G4200D_CS)
else
  ifneq (,$(TEST_L3G4200D_ADDR))
    CFLAGS += -DTEST_L3G4200D_ADDR=$(TEST_L3G4200D_ADDR)
  else
    # set random default
    CFLAGS += -DTEST_L3G4200D_ADDR=104
  endif
endif
ifneq (,$(TEST_L3G4200D_INT))
  CFLAGS += -DTEST_L3G4200D_INT=$(TEST_L3G4200D_INT)
else
  # set random default
  CFLAGS += -DTEST_L3G4200D_INT=GPIO_8
endif
ifneq (,$(TEST_L3G4200D_DRDY))
  CFLAGS += -DTEST_L3G4200D_DRDY=$(TEST_L3G4200D_DRDY)
else
  # set random default
  CFLAGS += -DTEST_L3G4200D_DRDY=GPIO_9
endif

include $(RIOTBASE)/Makefile.include
