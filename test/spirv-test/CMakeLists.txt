# Load Library Package
find_package(WEASEL)

include_directories(${SPIRV_INCLUDE_DIR})

# Include Directory
include_directories(${CMAKE_SOURCE_DIR}/include)

# Add Executable
file(GLOB SOURCES *.cpp)

add_executable(spirv-test ${SOURCES})

set(WEASEL_LIBS
    ${WEASEL_LIBS}
    ${SPIRV_LIB_DIR}/libLLVMSPIRVLib.a
)

target_link_libraries(spirv-test ${WEASEL_LIBS})
