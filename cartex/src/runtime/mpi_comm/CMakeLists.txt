target_sources(runtime PRIVATE runtime.cpp)
target_compile_options(runtime PUBLIC "$<$<COMPILE_LANGUAGE:CUDA>:SHELL:--default-stream per-thread>")
