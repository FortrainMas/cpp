cmake_minimum_required(VERSION 3.10)

project(CompressorProject)

find_package(ZLIB REQUIRED)

add_subdirectory(libcompressor)
add_subdirectory(compressor)
