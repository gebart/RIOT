add_riot_module(core
  assert.c
  atomic_c11.c
  atomic_sync.c
  bitarithm.c
  clist.c
  cond.c
  mutex.c
  priority_queue.c
  ringbuffer.c
  rmutex.c
  sched.c
  thread.c
  )
add_library(riot_core_flags INTERFACE)
target_include_directories(riot_core_flags INTERFACE include)
target_link_libraries(riot_flags INTERFACE riot_core_flags)
target_link_libraries(riot_core PUBLIC riot_core_flags)
add_library(RIOT::core ALIAS riot_core)

set(submodules init mbox msg msg_bus panic thread_flags)
foreach(submodule IN LISTS submodules)
  add_riot_module(core_${submodule} OBJECT ${submodule}.c)
  target_link_libraries(riot_module_core_${submodule} PUBLIC RIOT::core)
endforeach()
riot_use_module(core_init core_msg core_panic)
target_sources(RIOT INTERFACE $<TARGET_OBJECTS:RIOT::core>)
