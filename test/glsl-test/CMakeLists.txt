# Load Library Package
find_package(WEASEL)

# Include Directory
include_directories(${CMAKE_SOURCE_DIR}/include)

# Add Executable
file(GLOB SOURCES *.cpp)

add_executable(glsl-test ${SOURCES})

set(WEASEL_LIBS
    ${WEASEL_LIBS}
)

target_link_libraries(glsl-test ${WEASEL_LIBS})
