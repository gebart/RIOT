add_riot_module(posix_headers INTERFACE)
target_include_directories(riot_module_posix_headers_module_flags INTERFACE include)

add_riot_module(posix_inet OBJECT inet/inet.c)
riot_module_depends(posix_inet posix_headers)

add_riot_module(posix_select OBJECT select/posix_select.c)
riot_module_depends(posix_select posix_headers core_thread_flags xtimer)
# TODO conditionally depend on sock_async

add_riot_module(posix_semaphore OBJECT semaphore/posix_semaphore.c)
riot_module_depends(posix_semaphore posix_headers sema xtimer)

add_riot_module(posix_sleep OBJECT sleep/posix_sleep.c)
riot_module_depends(posix_sleep posix_headers ztimer_msec ztimer_usec)
# TODO conditionally depend on ztimer_periph_rtt

add_riot_module(posix_sockets OBJECT sockets/posix_sockets.c)
riot_module_depends(posix_sockets posix_headers bitfield random vfs xtimer)
