add_library(riot_board_${BOARD} STATIC
  include/deri/bsp/memory_map.h
  include/deri/bsp/devices.h
  dev/uart0.cpp
  dev/uart1.cpp
  )
target_include_directories(riot_board_${BOARD} PUBLIC include)
target_link_libraries(riot_board_${BOARD} PUBLIC
  deri_core
  deri_arch_${ARCH}
  riot_board
  deri_dev_sifive_uart
  )

add_subdirectory(${DERI_BSP_BASEDIR}/common/sifive ${CMAKE_CURRENT_BINARY_DIR}/common)
#target_link_libraries(riot_board_${BOARD} PUBLIC riot_board_sifive_common)
target_link_libraries(riot_board_${BOARD} PUBLIC deri_libc)
