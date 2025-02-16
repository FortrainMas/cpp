add_library(libcompressor STATIC src/libcompressor.cpp)

target_include_directories(libcompressor PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
