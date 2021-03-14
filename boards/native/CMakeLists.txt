add_riot_module(board
    board_init.c
    )
target_include_directories(riot_module_board INTERFACE include)
add_subdirectory(drivers)

# board_init uses native-led
riot_module_depends(board native_drivers)
