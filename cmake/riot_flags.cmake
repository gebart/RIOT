include_guard(GLOBAL)
include(riot_config)

target_compile_definitions(riot_module_RIOT INTERFACE "BOARD_$<UPPER_CASE:$<MAKE_C_IDENTIFIER:${BOARD}>>=\"${BOARD}\"" "RIOT_BOARD=BOARD_$<UPPER_CASE:$<MAKE_C_IDENTIFIER:${BOARD}>>")
target_compile_definitions(riot_module_RIOT INTERFACE "CPU_$<UPPER_CASE:$<MAKE_C_IDENTIFIER:${CPU}>>=\"${CPU}\"" "RIOT_CPU=CPU_$<UPPER_CASE:$<MAKE_C_IDENTIFIER:${CPU}>>")
target_compile_definitions(riot_module_RIOT INTERFACE "MCU_$<UPPER_CASE:$<MAKE_C_IDENTIFIER:${MCU}>>=\"${MCU}\"" "RIOT_MCU=MCU_$<UPPER_CASE:$<MAKE_C_IDENTIFIER:${MCU}>>")

target_compile_definitions(riot_module_RIOT INTERFACE "CPU_CORE_$<UPPER_CASE:$<MAKE_C_IDENTIFIER:${CPU_CORE}>>")
target_compile_definitions(riot_module_RIOT INTERFACE "CPU_MODEL_$<UPPER_CASE:$<MAKE_C_IDENTIFIER:${CPU_MODEL}>>")

# TODO allow multiple application names in one project
target_compile_definitions(riot_module_RIOT INTERFACE "RIOT_APPLICATION=\"${CMAKE_PROJECT_NAME}\"")

if (RIOT_DEVELHELP)
  target_compile_definitions(riot_module_RIOT INTERFACE DEVELHELP)
endif()

if (MSVC)
  # enable lots of warnings
  target_compile_options(riot_module_RIOT INTERFACE /W4)
  if (RIOT_WERROR)
    target_compile_options(riot_module_RIOT INTERFACE /WX)
  endif()
else() # Assume anything else is GCC/Clang compatible
  # lots of warnings
  target_compile_options(riot_module_RIOT INTERFACE -Wall -Wextra -pedantic)
  if (RIOT_WERROR)
    target_compile_options(riot_module_RIOT INTERFACE -Werror)
  endif()

  target_compile_options(riot_module_RIOT INTERFACE -Wdouble-promotion -Wconversion)
  target_compile_options(riot_module_RIOT INTERFACE -Wswitch-enum)
  target_compile_options(riot_module_RIOT INTERFACE -Wuninitialized -Winit-self)
  target_compile_options(riot_module_RIOT INTERFACE -Wformat=2)
  target_compile_options(riot_module_RIOT INTERFACE -fdiagnostics-color=always)

  if (CMAKE_C_COMPILER_ID STREQUAL GNU)
    # not supported by Clang
    target_compile_options(riot_module_RIOT INTERFACE -Wformat-truncation -Wformat-signedness)
  endif ()

  target_compile_options(riot_module_RIOT INTERFACE $<$<COMPILE_LANGUAGE:CXX>:-Woverloaded-virtual>)
  target_compile_options(riot_module_RIOT INTERFACE $<$<COMPILE_LANGUAGE:CXX>:-Weffc++>)

  # Avoid hiding unintended merging of globals
  target_compile_options(riot_module_RIOT INTERFACE -fno-common)

  # Splitting into separate sections helps keeping binary size down when combined
  # with --gc-sections
  target_compile_options(riot_module_RIOT INTERFACE -fdata-sections -ffunction-sections)
  if (NOT BOARD STREQUAL "native" OR NOT CMAKE_SYSTEM_NAME STREQUAL "Darwin")
    # Xcode linker does not support --gc-sections
    target_link_options(riot_module_RIOT INTERFACE LINKER:--gc-sections)
  endif()

  # Additional optimizations (experimental)
#  if (CMAKE_C_COMPILER_ID STREQUAL GNU)
#    target_compile_options(riot_flags -fstack-usage)
#    target_compile_options(riot_flags -findirect-inlining)
#    target_compile_options(riot_flags -finline-small-functions)
#  endif()
endif()
