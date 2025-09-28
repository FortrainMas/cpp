add_executable(tests
    test_foo.cpp
)

target_link_libraries(tests
    regex_lib
)

include(GoogleTest)
gtest_discover_tests(tests)
