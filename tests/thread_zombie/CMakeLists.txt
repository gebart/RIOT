add_executable(test_thread_zombie main.c)
target_link_libraries(test_thread_zombie PUBLIC RIOT::RIOT)

# For better testing use ps
#riot_target_depends(test_thread_zombie PUBLIC ps)
