# Load Library Package
find_package(WEASEL)
find_package(Vulkan REQUIRED)

# Add Executable
file(GLOB SOURCES
    *.cpp
)

add_executable(vulkan-test ${SOURCES})

target_link_libraries(vulkan-test Vulkan::Vulkan)
