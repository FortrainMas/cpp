add_library(libcompressor STATIC src/libcompressor.cpp)


target_include_directories(libcompressor PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)


include(${CMAKE_BINARY_DIR}/conan_toolchain.cmake OPTIONAL)


find_package(ZLIB REQUIRED)
find_package(BZip2 REQUIRED)


target_link_libraries(libcompressor PRIVATE ZLIB::ZLIB)
target_link_libraries(libcompressor PRIVATE BZip2::BZip2)
