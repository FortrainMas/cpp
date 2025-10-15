add_executable(tests
    test.cpp
    json.cpp
    toml.cpp
)

target_link_libraries(tests
    converter_lib
)

include(GoogleTest)
gtest_discover_tests(tests)
