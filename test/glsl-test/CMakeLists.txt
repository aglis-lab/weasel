# Load Library Package
find_package(WEASEL)

# Include Directory
include_directories(${CMAKE_SOURCE_DIR}/include)

# Add Executable
add_executable(glsl-test
    main.cpp
)

set(WEASEL_LIBS
    ${WEASEL_LIBS}
)

target_link_libraries(glsl-test ${WEASEL_LIBS})
