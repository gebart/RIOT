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

add_subdirectory(periph)
