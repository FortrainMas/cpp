cmake_minimum_required(VERSION 3.16)
project(warehouse)
set(CMAKE_CXX_STANDARD 20)

add_subdirectory(src)

# if(NOT WIN32)
#     include(${CMAKE_BINARY_DIR}/conan_toolchain.cmake)
#     find_package(GTest REQUIRED CONFIG)
#     enable_testing()
#     add_subdirectory(tests)
#     target_link_libraries(tests GTest::gtest_main)
# endif()
