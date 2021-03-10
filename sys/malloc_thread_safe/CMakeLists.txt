add_riot_module(malloc_thread_safe malloc_wrappers.c)
# use the wrapper functions for malloc and friends to provide thread-safety
target_link_options(riot_module_malloc_thread_safe INTERFACE -Wl,-wrap=malloc -Wl,-wrap=calloc -Wl,-wrap=realloc -Wl,-wrap=free)
