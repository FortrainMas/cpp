add_executable(compressor src/compressor.cpp)

target_include_directories(compressor PRIVATE
	${CMAKE_SOURCE_DIR}/libcompressor/include
)



find_package(spdlog REQUIRED)

target_link_libraries(compressor PRIVATE libcompressor)
target_link_libraries(compressor PRIVATE spdlog::spdlog)