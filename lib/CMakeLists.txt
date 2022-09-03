# Add Executable
file(GLOB SOURCES */*.cpp)

add_library(WeaselLib STATIC
    ${SOURCES}
)
