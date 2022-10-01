include_directories(${CMAKE_SOURCE_DIR}/include)

# Add Sources
file(GLOB SOURCES */*.cpp)

add_library(WeaselLib STATIC
    ${SOURCES}
)
