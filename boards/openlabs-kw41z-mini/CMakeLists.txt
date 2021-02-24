add_library(riot_board OBJECT
  board.c
  )
add_library(RIOT::board ALIAS riot_board)
add_library(riot_board_config INTERFACE)
add_library(RIOT::board::config ALIAS riot_board_config)
target_include_directories(riot_board_config INTERFACE include)
target_link_libraries(riot_board PUBLIC RIOT::flags RIOT::board::config)
target_link_libraries(riot_board PUBLIC
  RIOT::core
  RIOT::cpu::config
  )
target_sources(RIOT INTERFACE $<TARGET_OBJECTS:RIOT::board>)
