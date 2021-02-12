if(NOT CPU)
  message(FATAL_ERROR "CPU is not defined")
endif()
set(RIOT_CPU_DIR "${RIOT_CPU_BASEDIR}/${CPU}"
set(LINKER_SCRIPT "kinetis.ld")
add_link_options(${CPU_C_FLAGS} -T ${LINKER_SCRIPT} "-L${RIOT_CPU_DIR}/ldscripts")
