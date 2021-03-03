add_library(riot_core OBJECT
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
  include/assert.h
  include/bitarithm.h
  include/cib.h
  include/clist.h
  include/cond.h
  include/debug.h
  include/irq.h
  include/kernel_defines.h
  include/kernel_init.h
  include/list.h
  include/log.h
  include/macros/units.h
  include/macros/xtstr.h
  include/mbox.h
  include/msg.h
  include/msg_bus.h
  include/mutex.h
  include/native_sched.h
  include/panic.h
  include/priority_queue.h
  include/ringbuffer.h
  include/rmutex.h
  include/sched.h
  include/thread.h
  include/thread_flags.h
  )
target_link_libraries(riot_core PUBLIC
  RIOT::flags
  RIOT::cpu::config
  RIOT::modules::flags
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
