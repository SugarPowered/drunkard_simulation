add_library(common SHARED)
target_sources(common PRIVATE protocol.h)
target_compile_options(common PRIVATE "-Wall" "-Wextra" "-Wpedantic")