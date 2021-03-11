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

include(CheckCCompilerFlag)
include(CheckLinkerFlag)

if (MSVC)
  # enable lots of warnings
  target_compile_options(riot_module_RIOT INTERFACE /W4)
  if (RIOT_WERROR)
    target_compile_options(riot_module_RIOT INTERFACE /WX)
  endif()
else() # Assume anything else is GCC/Clang compatible

  target_compile_options(riot_module_RIOT INTERFACE
      # Force the C compiler to not ignore signed integer overflows
      # Background:   In practise signed integers overflow consistently and wrap
      #               around to the lowest number. But this is undefined behaviour.
      #               Branches that rely on this undefined behaviour will be silently
      #               optimized out. For details, have a look at
      #               https://gcc.gnu.org/bugzilla/show_bug.cgi?id=30475
      # Note:         Please do not add new code that relies on this undefined
      #               behaviour, even though this flag makes your code work. There are
      #               safe ways to check for signed integer overflow.
      -fwrapv
      # Enable warnings for code relying on signed integers to overflow correctly
      # (see above for details).
      # Note:         This warning is sadly not reliable, thus -fwrapv cannot be
      #               dropped in favor of this
      -Wstrict-overflow

      # Forbid common symbols to prevent accidental aliasing.
      -fno-common

      # Place data and functions into their own sections. This helps the linker
      # garbage collection to remove unused symbols when linking statically.
      -ffunction-sections -fdata-sections

      # Enable all default warnings and all extra warnings
      -Wall -Wextra

      # Warn if a user-supplied include directory does not exist.
      -Wmissing-include-dirs

      # Fast-out on old style function definitions.
      # They cause unreadable error compiler errors on missing semicolons.
      # Worse yet they hide errors by accepting wildcard argument types.
      # (These flags only have meaning in C language)
      $<$<COMPILE_LANGUAGE:C>:-Wstrict-prototypes>
      $<$<COMPILE_LANGUAGE:C>:-Wold-style-definition>
      )
  set(cflags_to_try
      # Add `-fno-delete-null-pointer-checks` flag iff the compiler supports it.
      # GCC removes moves tests whether `x == NULL`, if previously `x` or even `x->y` was accessed.
      # 0x0 might be a sane memory location for embedded systems, so the test must not be removed.
      # Right now clang does not use the *delete-null-pointer* optimization, and does not understand the parameter.
      # Related issues: #628, #664.
      -fno-delete-null-pointer-checks
      # Enable additional checks for printf/scanf format strings
      # These are not universally supported by all compiler versions, so must be tested for
      -Wformat=2
      -Wformat-overflow
      -Wformat-truncation
      -Wformat-signedness

      # Additional optimizations (experimental)
      #-fstack-usage
      #-findirect-inlining
      #-finline-small-functions

      # Additional warnings
      #-Wuninitialized
      #-Winit-self
      #-Wswitch-enum
      #-Wdouble-promotion
      #-Wconversion
  )
  if (RIOT_COMPRESS_DEBUG)
    list(APPEND cflags_to_try -gz)
  endif()
  if (RIOT_CC_COLOR)
    list(APPEND cflags_to_try -fdiagnostics-color=always)
  else()
    list(APPEND cflags_to_try -fno-diagnostics-color)
  endif()

  if(BOARD STREQUAL "native" AND CMAKE_SIZEOF_VOID_P EQUAL 8)
    # We should really be using a toolchain file for native too
    target_compile_options(riot_module_RIOT INTERFACE -m32)
    target_link_options(riot_module_RIOT INTERFACE -m32)
  endif()

  foreach(flag IN LISTS cflags_to_try)
    string(TOUPPER "${flag}" flag_u)
    string(MAKE_C_IDENTIFIER "${flag_u}" flag_c)
    check_c_compiler_flag("${flag}" HAS${flag_c})
    if (HAS${flag_c})
      target_compile_options(riot_module_RIOT INTERFACE "${flag}")
    endif()
  endforeach()

  if (RIOT_WERROR)
    target_compile_options(riot_module_RIOT INTERFACE -Werror)
  endif()

#  target_compile_options(riot_module_RIOT INTERFACE $<$<COMPILE_LANGUAGE:CXX>:-Woverloaded-virtual>)
#  target_compile_options(riot_module_RIOT INTERFACE $<$<COMPILE_LANGUAGE:CXX>:-Weffc++>)

  # Xcode linker does not support --gc-sections, but uses -dead_strip
  check_linker_flag(C "LINKER:--gc-sections" HAS_LINKER_GC_SECTIONS)
  if (HAS_LINKER_GC_SECTIONS)
    target_link_options(riot_module_RIOT INTERFACE "LINKER:--gc-sections")
  else()
    check_linker_flag(C "LINKER:-dead_strip" HAS_LINKER_DEAD_STRIP)
    if (HAS_LINKER_DEAD_STRIP)
      target_link_options(riot_module_RIOT INTERFACE "LINKER:-dead_strip")
    endif()
  endif()
endif()
