if(NOT CPU)
  message(FATAL_ERROR "CPU is not defined")
endif()

if(NOT CPU_MODEL)
  message(FATAL_ERROR "CPU_MODEL is not set")
endif()

string(TOUPPER "${CPU_MODEL}" CPU_MODEL)
if (CPU_MODEL MATCHES "^(M|K|S9)K([ELMSVW]?|EA)([0-9]*)([A-Z])([NX]?)([0-9][0-9M]?[0-9]?)([ABZ]?)(.*)$")
  set(KINETIS_QUALIFICATION ${CMAKE_MATCH_1})
  set(KINETIS_SERIES ${CMAKE_MATCH_2})
  set(KINETIS_FAMILY_SUBFAMILY ${CMAKE_MATCH_3})
  set(KINETIS_CORE ${CMAKE_MATCH_4})
  set(KINETIS_FLEXMEM ${CMAKE_MATCH_5})
  set(KINETIS_ROMSIZE ${CMAKE_MATCH_6})
  set(KINETIS_MASKREV ${CMAKE_MATCH_7})
  if ("${CMAKE_MATCH_8}" MATCHES "^([CMV])(..)([0-9]*).*$")
    set(KINETIS_TEMPRANGE ${CMAKE_MATCH_1})
    set(KINETIS_PACKAGE ${CMAKE_MATCH_2})
    set(KINETIS_SPEED ${CMAKE_MATCH_3})
  else()
    message(FATAL_ERROR "Invalid Kinetis CPU_MODEL: ${CPU_MODEL}")
  endif()
else()
  message(FATAL_ERROR "Invalid Kinetis CPU_MODEL: ${CPU_MODEL}")
endif()
if (KINETIS_FAMILY_SUBFAMILY MATCHES "([0-9]?)([0-9]?)")
  set(KINETIS_FAMILY ${CMAKE_MATCH_0})
  set(KINETIS_SUBFAMILY ${CMAKE_MATCH_1})
endif()
if (KINETIS_ROMSIZE MATCHES "^([0-9]+)M([0-9])$")
  math(EXPR KINETIS_ROMSIZE "${CMAKE_MATCH_0} * 1024 + ${CMAKE_MATCH_1} * 128")
endif()

if (NOT KINETIS_INFO_DEBUG)
  message("Qualification: ${KINETIS_QUALIFICATION}")
  message("Series: ${KINETIS_SERIES}")
  message("Family: ${KINETIS_FAMILY}")
  message("Subfamily: ${KINETIS_SUBFAMILY}")
  message("Core: ${KINETIS_CORE}")
  message("Flexmem: ${KINETIS_FLEXMEM}")
  message("ROM size: ${KINETIS_ROMSIZE}")
  message("Mask rev: ${KINETIS_MASKREV}")
  message("Temp range: ${KINETIS_TEMPRANGE}")
  message("Package: ${KINETIS_PACKAGE}")
  message("Speed: ${KINETIS_SPEED}")
endif()

# RAM sizes are a bit arbitrary, but are usually dependent on ROM size and core speed.
# This block is not complete, but it does cover all the CPUs supported by RIOT, and then some.
if (KINETIS_SERIES STREQUAL "K")
  if (KINETIS_ROMSIZE GREATER_EQUAL 2048)
    if (KINETIS_SUBFAMILY GREATER_EQUAL 7)
      # OCRAM in K27, K28 is not mapped here.
      set(KINETIS_RAMSIZE 512)
    else()
      set(KINETIS_RAMSIZE 256)
    endif()
  elseif (KINETIS_ROMSIZE GREATER_EQUAL 1024)
    if (KINETIS_SUBFAMILY GREATER_EQUAL 3)
      set(KINETIS_RAMSIZE 256)
    else()
      set(KINETIS_RAMSIZE 128)
    endif()
  elseif (KINETIS_ROMSIZE GREATER_EQUAL 512)
    if (KINETIS_SPEED EQUAL 5)
      set(KINETIS_RAMSIZE 64)
    elseif (KINETIS_SUBFAMILY EQUAL 4)
      set(KINETIS_RAMSIZE 192)
    else()
      set(KINETIS_RAMSIZE 128)
    endif()
  elseif (KINETIS_ROMSIZE GREATER_EQUAL 256)
    if (KINETIS_SPEED EQUAL 5)
      set(KINETIS_RAMSIZE 32)
    elseif (KINETIS_SPEED EQUAL 15)
      set(KINETIS_RAMSIZE 256)
    elseif (KINETIS_FAMILY_SUBFAMILY EQUAL 22)
      set(KINETIS_RAMSIZE 48)
    elseif (KINETIS_FAMILY_SUBFAMILY EQUAL 24)
      set(KINETIS_RAMSIZE 256)
    elseif (KINETIS_FAMILY_SUBFAMILY EQUAL 20)
      set(KINETIS_RAMSIZE 64)
    endif()
  elseif (KINETIS_ROMSIZE GREATER_EQUAL 128)
    if (KINETIS_FAMILY EQUAL 0)
      set(KINETIS_RAMSIZE 16)
    elseif (KINETIS_SUBFAMILY EQUAL 0 AND KINETIS_SPEED EQUAL 5)
      set(KINETIS_RAMSIZE 16)
    elseif (KINETIS_CORE STREQUAL "F")
      set(KINETIS_RAMSIZE 24)
      set(KINETIS_SRAM_L_SIZE 8)
    else()
      set(KINETIS_RAMSIZE 32)
    endif()
  elseif (KINETIS_ROMSIZE GREATER_EQUAL 64)
    set(KINETIS_RAMSIZE 16)
  endif()
  if (NOT KINETIS_SRAM_L_SIZE)
    # A majority of the models have this ratio of SRAM_L vs total SRAM size
    math(EXPR KINETIS_SRAM_L_SIZE "${KINETIS_RAMSIZE} / 2")
  endif()
  if (KINETIS_SRAM_L_SIZE GREATER 64)
    # There seems to be a cap on SRAM_L at 64 kB across the whole K series
    set(KINETIS_SRAM_L_SIZE 64)
  endif()
elseif(KINETIS_SERIES STREQUAL "L")
  if (KINETIS_FAMILY EQUAL 8)
    # KL81, KL82
    set(KINETIS_RAMSIZE 96)
  elseif (KINETIS_SUBFAMILY EQUAL 7)
    # KL17, KL27
    if (KINETIS_ROMSIZE EQUAL 256)
      set(KINETIS_RAMSIZE 32)
    else()
      math(EXPR KINETIS_RAMSIZE "${KINETIS_ROMSIZE} / 4")
    endif()
  elseif (KINETIS_FAMILY_SUBFAMILY EQUAL 28)
    # KL28
    set(KINETIS_RAMSIZE 128)
  elseif (KINETIS_FAMILY_SUBFAMILY EQUAL 03)
    # KL03
    set(KINETIS_RAMSIZE 2)
  else()
    math(EXPR KINETIS_RAMSIZE "${KINETIS_ROMSIZE} / 8")
  endif()
  math(EXPR KINETIS_SRAM_L_SIZE "${KINETIS_RAMSIZE} / 4")
elseif (KINETIS_SERIES STREQUAL "W")
  math(EXPR KINETIS_RAMSIZE "${KINETIS_ROMSIZE} / 8")
  if (KINETIS_CORE STREQUAL "D")
    math(EXPR KINETIS_SRAM_L_SIZE "${KINETIS_RAMSIZE} / 2")
  elseif (KINETIS_FAMILY EQUAL 0)
    # 01Z
    set(KINETIS_SRAM_L_SIZE 4)
  elseif (KINETIS_SUBFAMILY EQUAL 0)
    # 20Z, 30Z, 40Z
    set(KINETIS_SRAM_L_SIZE 4)
  elseif (KINETIS_SUBFAMILY EQUAL 1)
    # 21Z, 31Z, 41Z
    math(EXPR KINETIS_RAMSIZE "${KINETIS_ROMSIZE} / 4")
    math(EXPR KINETIS_SRAM_L_SIZE "${KINETIS_RAMSIZE} / 4")
  else()
    # TODO: KW35/36/37/38/39
    message(FATAL_ERROR "Unknown Kinetis W model: ${CPU_MODEL}")
  endif()
elseif (KINETIS_SERIES EQUAL EA)
  math(EXPR KINETIS_RAMSIZE "${KINETIS_ROMSIZE} / 8")
  math(EXPR KINETIS_SRAM_L_SIZE "${KINETIS_RAMSIZE} / 4")
endif()

if (NOT KINETIS_RAMSIZE)
  message(FATAL_ERROR "Missing KINETIS_RAMSIZE case")
endif()
if (NOT KINETIS_SRAM_L_SIZE)
  message(FATAL_ERROR "Missing KINETIS_SRAM_L_SIZE case")
endif()
