add_executable(rw_server)
target_sources(rw_server PRIVATE main.c server.c simulation.c) 
target_compile_options(rw_server PUBLIC "-std=c11")
target_compile_options(rw_server PRIVATE "-Wall" "-Wextra" "-Wpedantic")
