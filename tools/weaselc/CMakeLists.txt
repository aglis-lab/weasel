find_package(WEASEL)

include_directories(${CMAKE_SOURCE_DIR}/include)

add_executable(weaselc main.cpp)

target_link_libraries(weaselc ${WEASEL_LIBS} fmt::fmt glog::glog)
