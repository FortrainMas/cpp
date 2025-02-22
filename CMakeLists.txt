cmake_minimum_required(VERSION 3.10)

project(CompressorProject)

enable_testing()

add_subdirectory(libcompressor)
add_subdirectory(compressor)
add_subdirectory(libcompressor/tests)