include(${CMAKE_BINARY_DIR}/conan_toolchain.cmake)

enable_testing()

add_executable(libcompressortest libcompressortest.cpp)

target_include_directories(libcompressortest PRIVATE
    ${CMAKE_SOURCE_DIR}/libcompressor/include
)

find_package(GTest REQUIRED)

target_link_libraries(libcompressortest PRIVATE GTest::GTest GTest::Main)
target_link_libraries(libcompressortest PRIVATE libcompressor)

add_test(NAME libcompressortest COMMAND libcompressortest)
