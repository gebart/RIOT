add_riot_module(cpu
    async_read.c
    irq_cpu.c
    native_cpu.c
    panic.c
    startup.c
    syscalls.c
    tramp.S
)
target_include_directories(riot_module_cpu INTERFACE "include")

add_riot_module(stdio_native stdio_native/stdio_native.c)

# UART and pm is needed by startup.c
riot_module_depends(cpu periph_pm periph_uart)

# syscalls use dlsym
target_link_libraries(riot_module_cpu INTERFACE ${CMAKE_DL_LIBS})

add_subdirectory(periph)

# UART is needed by startup.c
riot_module_depends(cpu periph_uart)
