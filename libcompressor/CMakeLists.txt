add_library(libcompressor STATIC src/libcompressor.cpp)

target_include_directories(libcompressor PUBLIC 
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}> 
)

set_target_properties(libcompressor PROPERTIES OUTPUT_NAME "libcompressor")

include(${CMAKE_BINARY_DIR}/conan_toolchain.cmake OPTIONAL)

find_package(ZLIB REQUIRED)
find_package(BZip2 REQUIRED)

target_link_libraries(libcompressor PRIVATE ZLIB::ZLIB BZip2::BZip2)

include(GNUInstallDirs)

install(TARGETS libcompressor
    EXPORT libcompressorTargets
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

install(DIRECTORY include/ DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})

install(EXPORT libcompressorTargets
    FILE libcompressorConfig.cmake
    NAMESPACE libcompressor::
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/libcompressor
)
