add_library(libcompressor STATIC src/libcompressor.cpp)

include(${CMAKE_BINARY_DIR}/conan_toolchain.cmake OPTIONAL)

target_include_directories(libcompressor PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

find_package(ZLIB REQUIRED)  
target_link_libraries(libcompressor PRIVATE ZLIB::ZLIB)
