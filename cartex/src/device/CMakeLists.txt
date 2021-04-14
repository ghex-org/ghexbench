add_library(device)
target_link_libraries(device PRIVATE cartex)
compile_as_cuda(set_device.cpp)
target_sources(device PRIVATE set_device.cpp)
