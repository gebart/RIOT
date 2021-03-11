add_riot_module(board
    board_init.c
    )
target_include_directories(riot_module_board INTERFACE include)
add_subdirectory(drivers)
