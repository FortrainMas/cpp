/**
 * @file compressor.cpp
 * @brief Command-line utility for compressing input strings using libcompressor.
 *
 * This program takes two arguments:
 *  1. Compression algorithm ("zlib" or "bzip").
 *  2. Input string to be compressed.
 *
 * It compresses the input string using the specified algorithm and outputs
 * the compressed data in hexadecimal format to stdout, one byte at a time.
 * Errors are reported to stderr.
 *
 * ## Error Cases:
 * - Insufficient arguments.
 * - Invalid compression algorithm.
 * - Compression failure (empty output buffer).
 *
 * In case of an error, an error message is printed, and the program exits with EXIT_FAILURE.
 * On success, the program exits with EXIT_SUCCESS.
 */

#include <iostream>
#include <string>
#include <cstring>
#include <cstdlib>
#include "libcompressor/libcompressor.hpp"

int main(int argc, char **argv) {
    if (argc != 3) {
        std::cerr << "Error: Insufficient arguments. Usage: compressor <algorithm> <input_string>" << std::endl;
        return EXIT_FAILURE;
    }
 
    std::string algorithm = argv[1];
    libcompressor_CompressionAlgorithm algo;
 
    if (algorithm == "zlib") {
        algo = libcompressor_Zlib;
    } else if (algorithm == "bzip") {
        algo = libcompressor_Bzip;
    } else {
        std::cerr << "Error: Invalid compression algorithm. Use 'zlib' or 'bzip'." << std::endl;
        return EXIT_FAILURE;
    }
 
    std::string input_str = argv[2];
    libcompressor_Buffer input;
    input.data = const_cast<char*>(input_str.c_str());
    input.size = input_str.size();

    libcompressor_Buffer output = libcompressor_compress(algo, input);
 
    if (output.size == 0) {
        std::cerr << "Error: Compression failed, output buffer is empty." << std::endl;
        return EXIT_FAILURE;
    }

    for (int i = 0; i < output.size; ++i) {
        std::printf("%.2hhx", static_cast<unsigned char>(output.data[i]));
    }
    std::printf("\n");

    return EXIT_SUCCESS;
}