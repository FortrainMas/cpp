add_library(regex_lib
	regex/regex.cpp
	regex/nfa.cpp
	regex/parser.cpp
)

target_include_directories(regex_lib PUBLIC ${PROJECT_SOURCE_DIR}/include)

add_executable(main main.cpp)
target_link_libraries(main PRIVATE regex_lib)
