find_package(WEASEL)

add_executable(weaselc weaselc.cpp)

target_link_libraries(weaselc ${WEASEL_LIBS})
