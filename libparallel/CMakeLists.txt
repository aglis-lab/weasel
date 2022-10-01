# Include Directory
include_directories(${CMAKE_SOURCE_DIR}/libparallel/include)

# Load Library Package
find_package(Vulkan REQUIRED)

# Add Sources
file(GLOB SOURCES src/*.cpp)

# Sources
add_library(WeaselParallel STATIC
    ${SOURCES}
)

target_link_libraries(WeaselParallel Vulkan::Vulkan)
