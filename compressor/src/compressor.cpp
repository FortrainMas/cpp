#include <stdlib.h>
#include <iostream>
#include <string.h>

#include "libcompressor/libcompressor.hpp"

int main(int argc, char ** argv){
	libcompressor_CompressionAlgorithm algo = libcompressor_Zlib;
	
	libcompressor_Buffer input;
    input.data = argv[1];
    input.size = strlen(argv[1]);

	libcompressor_compress(algo, input);
	

	std::cout << "works" << std::endl;

	return 0;
}
