cmake_minimum_required(VERSION 3.21)
project(message_broker LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(ZeroMQ CONFIG REQUIRED)
find_package(cppzmq CONFIG REQUIRED)
find_package(nlohmann_json CONFIG REQUIRED)
find_package(yaml-cpp CONFIG REQUIRED)

add_executable(client
    src/main.cpp
)

target_include_directories(client
    PRIVATE
        ${cppzmq_INCLUDE_DIRS}
    public:
        ${PROJECT_SOURCE_DIR}/include
)


target_link_libraries(client
    PRIVATE
        libzmq-static
        cppzmq
        nlohmann_json::nlohmann_json
        yaml-cpp::yaml-cpp
)
