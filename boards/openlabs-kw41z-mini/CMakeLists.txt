add_riot_module(board OBJECT
  board.c
  )
add_library(RIOT::board ALIAS riot_module_board)
add_library(riot_board_config INTERFACE)
add_library(RIOT::board::config ALIAS riot_board_config)
target_include_directories(riot_board_config INTERFACE include)
target_link_libraries(riot_flags INTERFACE riot_board_config)
