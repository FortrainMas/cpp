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
