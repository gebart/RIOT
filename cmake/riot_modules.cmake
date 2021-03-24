include_guard(GLOBAL)
include(riot_functions)
include(riot_config)

# RIOT::RIOT is the top level target which is meant to be used from the
# application to add a dependency on RIOT
add_riot_module(RIOT)
add_library(RIOT::RIOT ALIAS riot_module_RIOT)
target_compile_definitions(riot_module_RIOT INTERFACE RIOT_VERSION="${PROJECT_VERSION}")

# Abstract interface modules to allow the user to select the stdio implementation
add_riot_module(stdio)
riot_module_depends(RIOT stdio)
add_riot_module(stdin)
# Note that in order to reduce binary bloat, stdin must be explicitly enabled
# in each application that uses terminal input

if (NOT RIOT_STDIO)
  message(FATAL_ERROR "No stdio implementation specified")
endif()
riot_module_depends(stdio ${RIOT_STDIO})

# The libc module is an abstract interface that allows us to select C library
# implementation in the build configuration
add_riot_module(libc)
riot_module_depends(RIOT libc)

# Abstract interface module for depending on C math functions
include(CheckCSourceCompiles)
message(CHECK_START "Checking for math.h support")
set(LIBM_TEST_SOURCE "#include<math.h>\nfloat f; int main(){sqrt(f);return 0;}")
check_c_source_compiles("${LIBM_TEST_SOURCE}" HAVE_MATH)
if(HAVE_MATH)
  set(LIBM_LIBRARIES)
else()
  set(CMAKE_REQUIRED_LIBRARIES m)
  check_c_source_compiles("${LIBM_TEST_SOURCE}" HAVE_LIBM_MATH)
  unset(CMAKE_REQUIRED_LIBRARIES)
  if(NOT HAVE_LIBM_MATH)
    message(CHECK_FAIL "Unable to use C standard math library functions")
  else()
    message(CHECK_PASS "done")
    set(LIBM_LIBRARIES m)
  endif()
endif()

add_riot_module(libm)
target_link_libraries(riot_module_libm INTERFACE ${LIBM_LIBRARIES})

if (NOT BOARD STREQUAL "native")
  if (NOT RIOT_LIBC)
    message(FATAL_ERROR "No libc implementation specified")
  endif()
  riot_module_depends(libc ${RIOT_LIBC})

  if (NOT RIOT_SYSCALLS)
    message(FATAL_ERROR "No syscalls implementation specified")
  endif()
  riot_module_depends(libc ${RIOT_SYSCALLS})
endif()

if (RIOT_USEMODULE_EXTRA)
  riot_module_depends(RIOT ${RIOT_USEMODULE_EXTRA})
endif()
