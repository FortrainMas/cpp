cmake_minimum_required(VERSION 3.10)

project(CompressorProject)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(MSVC)
    add_compile_options(/W4 /WX)
else()
    add_compile_options(-Wall -Wextra -Wpedantic -Werror)
endif()

enable_testing()

add_subdirectory(libcompressor)
add_subdirectory(compressor)
add_subdirectory(libcompressor/tests)



find_package(Doxygen REQUIRED)

set(DOXYGEN_OUTPUT_DIR ${CMAKE_BINARY_DIR}/docs)

set(DOXYGEN_CONFIG_FILE ${CMAKE_BINARY_DIR}/Doxyfile)

file(WRITE ${DOXYGEN_CONFIG_FILE}
    "PROJECT_NAME = Compressor\n"
    "PROJECT_BRIEF = Library for data compression\n"
    "OUTPUT_DIRECTORY = ${DOXYGEN_OUTPUT_DIR}\n"
    "INPUT = ${CMAKE_SOURCE_DIR}\n"
    "FILE_PATTERNS = *.cpp *.hpp *.h\n"
    "RECURSIVE = YES\n"
    "EXCLUDE_PATTERNS = */build/* */test/*\n"
    "GENERATE_HTML = NO\n"
    "GENERATE_LATEX = NO\n"
    "GENERATE_MAN = YES\n"
    "GENERATE_XML = NO\n"
    "HAVE_DOT = YES\n"
    "DOT_GRAPH_MAX_NODES = 50\n"
    "CALL_GRAPH = YES\n"
    "CALLER_GRAPH = YES\n"
    "UML_LOOK = YES\n"
    "TEMPLATE_RELATIONS = YES\n"
)

add_custom_target(doc
    COMMAND Doxygen::doxygen ${DOXYGEN_CONFIG_FILE}
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
    COMMENT "Generating API documentation with Doxygen"
    VERBATIM
)