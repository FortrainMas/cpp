add_library(converter_lib
    converter/baseConverter.cpp
    converter/json.cpp
    converter/toml.cpp
    converter/xml.cpp
)

target_include_directories(converter_lib PUBLIC ${PROJECT_SOURCE_DIR}/include)

add_executable(main main.cpp)
target_link_libraries(main converter_lib)
