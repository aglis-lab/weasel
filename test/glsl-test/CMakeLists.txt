# Load Library Package
find_package(WEASEL)

# Add Executable
file(GLOB SOURCES *.cpp)

add_executable(glsl-test ${SOURCES})

set(WEASEL_LIBS
    ${WEASEL_LIBS}
)

target_link_libraries(glsl-test ${WEASEL_LIBS})
