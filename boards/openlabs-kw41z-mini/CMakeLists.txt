add_library(riot_board STATIC
  board.c
  )
add_library(RIOT::board ALIAS riot_board)
target_link_libraries(riot_board PUBLIC riot_flags)
target_include_directories(riot_board PUBLIC include)
target_link_libraries(riot_board PUBLIC
  riot_core
  riot_cpu
  )

#add_subdirectory(${DERI_BSP_BASEDIR}/common/sifive ${CMAKE_CURRENT_BINARY_DIR}/common)
#target_link_libraries(riot_board_${BOARD} PUBLIC riot_board_sifive_common)
#target_link_libraries(riot_board_${BOARD} PUBLIC deri_libc)
