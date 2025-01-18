add_library(pipe-lib SHARED)
target_sources(pipe-lib PRIVATE pipe.c)
target_compile_options(pipe-lib PRIVATE "-Wall" "-Wextra" "-Wpedantic")