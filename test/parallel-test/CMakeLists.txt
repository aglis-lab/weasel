# Load Library Package
find_package(WEASEL)
find_package(Vulkan REQUIRED)

include_directories(${CMAKE_SOURCE_DIR}/libparallel/include)

# Add Executable
file(GLOB SOURCES
    *.cpp
)

add_executable(parallel-test ${SOURCES})

set(PARALLEL_LIBS
    WeaselParallel
    Vulkan::Vulkan
)
target_link_libraries(parallel-test ${PARALLEL_LIBS})
