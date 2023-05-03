include_directories(${CMAKE_SOURCE_DIR}/include)

# Add Sources
file(GLOB SOURCES */*.cpp)

add_library(WeaselLib STATIC
    ${SOURCES}
)

target_link_libraries(WeaselLib glog::glog fmt::fmt Threads::Threads)
