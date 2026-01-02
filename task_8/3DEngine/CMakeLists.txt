add_library(3DEngine STATIC
    "src/SimpleEngine/SimpleEngine.cpp"
    "src/BinaryEngine/BinaryEngine.cpp"
    "src/SpatHashEngine/SpatHashEngine.cpp"
    "src/SOAEngine/SOAEngine.cpp"
    "src/MultithreadEngine/MultithreadEngine.cpp"
    "src/SIMDEngine/SIMDEngine.cpp")

find_package(Threads REQUIRED)
find_package(CLI11 CONFIG REQUIRED)

target_compile_options(3DEngine PRIVATE
    -mavx2
    -mfma
)

target_include_directories(3DEngine PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)

target_link_libraries(3DEngine PUBLIC Threads::Threads)


add_executable(engine main.cpp)
target_link_libraries(engine PRIVATE 3DEngine CLI11::CLI11)


add_executable(generator generator.cpp)
target_link_libraries(generator PRIVATE 3DEngine)


add_executable(transformer transformer.cpp)
target_link_libraries(transformer PRIVATE 3DEngine)

# add_subdirectory(tests)
