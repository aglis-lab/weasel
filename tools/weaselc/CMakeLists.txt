find_package(WEASEL)

include_directories(${CMAKE_SOURCE_DIR}/include)

add_executable(weaselc main.cpp)

target_link_libraries(weaselc PRIVATE ${WEASEL_LIBS} fmt::fmt glog::glog opentelemetry-cpp::api opentelemetry-cpp::ext opentelemetry-cpp::sdk opentelemetry-cpp::logs)
