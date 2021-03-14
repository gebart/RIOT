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
target_include_directories(riot_module_core INTERFACE include)

set(submodules init mbox msg msg_bus panic thread_flags)
foreach(submodule IN LISTS submodules)
  add_riot_module(core_${submodule} ${submodule}.c)
  riot_module_depends(core_${submodule} core)
endforeach()
riot_module_depends(RIOT core core_init core_msg core_panic)

add_riot_module(core_idle_thread)
add_riot_module(no_idle_thread)

# TODO Make this conditional
riot_module_depends(core core_idle_thread)
