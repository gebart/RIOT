cmake_minimum_required(VERSION 3.18)
project(RIOT
  VERSION 2021.01
  HOMEPAGE_URL https://github.com/RIOT-OS/RIOT
  LANGUAGES C CXX ASM
  )

if (NOT BOARD)
  message(FATAL_ERROR "Define BOARD to select platform: cmake -DBOARD=foo")
endif ()

set(BOARD "${BOARD}" CACHE STRING "Which board type the application should run on")

if (CMAKE_SOURCE_DIR STREQUAL CMAKE_CURRENT_LIST_DIR)
  message(NOTICE "Umbrella project build")
  message(FATAL_ERROR "Not yet implemented")
else()
  set(CMAKE_C_STANDARD 11)
  set(CMAKE_C_STANDARD_REQUIRED ON)
  set(CMAKE_C_EXTENSIONS ON)

  set(CMAKE_CXX_STANDARD 20)
  set(CMAKE_CXX_STANDARD_REQUIRED ON)
  set(CMAKE_CXX_EXTENSIONS OFF)

  include(riot_functions)
  include(riot_targets)
  include(riot_modules)
  include(riot_flags)

  if (CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME AND BUILD_TESTING)
    add_subdirectory(tests)
  endif ()

  add_subdirectory(core)
  add_subdirectory(cpu)
  add_subdirectory(boards)
  add_subdirectory(drivers)
  add_subdirectory(sys)

  get_target_property(usemodule riot_modules INTERFACE_RIOT_USEMODULE)
  message(VERBOSE "Used RIOT modules: ${usemodule}")
endif()

#add_compile_options($<$<COMPILE_LANGUAGE:CXX>:-fno-use-cxa-atexit>)
#add_compile_options($<$<COMPILE_LANGUAGE:CXX>:-fno-rtti>)
#add_compile_options($<$<COMPILE_LANGUAGE:CXX>:-fno-exceptions>)

