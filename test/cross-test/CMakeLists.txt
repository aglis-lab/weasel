find_package(WEASEL)

include_directories(${SPIRV_CROSS_INCLUDE_DIR})

add_executable(cross-test main.cpp)

set(WEASEL_LIBS
    ${WEASEL_LIBS}
    ${SPIRV_CROSS_LIB_DIR}/libspirv-cross-c.a
    ${SPIRV_CROSS_LIB_DIR}/libspirv-cross-core.a
    ${SPIRV_CROSS_LIB_DIR}/libspirv-cross-cpp.a
    ${SPIRV_CROSS_LIB_DIR}/libspirv-cross-glsl.a
    ${SPIRV_CROSS_LIB_DIR}/libspirv-cross-hlsl.a
    ${SPIRV_CROSS_LIB_DIR}/libspirv-cross-msl.a
    ${SPIRV_CROSS_LIB_DIR}/libspirv-cross-reflect.a
    ${SPIRV_CROSS_LIB_DIR}/libspirv-cross-util.a
)

target_link_libraries(cross-test ${WEASEL_LIBS})
