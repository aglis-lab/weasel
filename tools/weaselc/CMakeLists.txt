find_package(WEASEL)

add_executable(weaselc main.cpp)

target_link_libraries(weaselc ${WEASEL_LIBS})
