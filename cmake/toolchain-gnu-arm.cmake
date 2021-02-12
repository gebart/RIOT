set(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_SYSTEM_VERSION 1)
set(CMAKE_SYSTEM_PROCESSOR arm)

set(CMAKE_CROSSCOMPILING true)

# We can not link without extra linker flags but we want to let the build
# system decide those linker flags in the board configuration, so here we use a
# static library for try_compile tests in CMake instead of the default which is
# building an executable.
set(CMAKE_TRY_COMPILE_TARGET_TYPE STATIC_LIBRARY)

set(CMAKE_C_COMPILER "arm-none-eabi-gcc")
set(CMAKE_CXX_COMPILER "arm-none-eabi-g++")

# never look in the target sysroot for programs and tools
set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
# look for libraries and headers on the target sysroot only
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_PACKAGE ONLY)
