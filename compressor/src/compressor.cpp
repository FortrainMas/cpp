/**
 * @file compressor.cpp
 * @brief Command-line utility for compressing input strings using libcompressor.
 *
 * This program takes two arguments: 
 *  1. Compression algorithm ("zlib" or "bzip").
 *  2. Input string to be compressed.
 *
 * It compresses the input string using the specified algorithm and outputs 
 * the compressed data in hexadecimal format to stdout. Errors are logged 
 * using spdlog with level ERROR.
 *
 * ## Error Cases:
 * - Insufficient arguments.
 * - Invalid compression algorithm.
 * - Compression failure (empty output buffer).
 *
 * In case of an error, an error message is logged, and the program exits with EXIT_FAILURE.
 * On success, the program exits with EXIT_SUCCESS.
 */


#include <stdlib.h>
#include <iostream>
#include <string.h>

#include "libcompressor/libcompressor.hpp"

int main(int argc, char ** argv){
    libcompressor_CompressionAlgorithm algo = libcompressor_Bzip;
    
    libcompressor_Buffer input;
    input.data = const_cast<char*>("test_string");
    input.size = strlen(input.data);
    
    libcompressor_Buffer output = libcompressor_compress(algo, input);
    

    std::cout << "{";
    for (size_t i = 0; i < output.size; ++i) {
        std::cout << "0x" << std::hex << (0xFF & static_cast<unsigned char>(output.data[i]));
        if (i != output.size - 1) {
            std::cout << ", ";
        }
    }
    std::cout << "}" << std::endl;

    return 0;
}
