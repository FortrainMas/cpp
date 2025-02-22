#include <stdlib.h>
#include <cstring>

#include "libcompressor/libcompressor.hpp"
#include "zlib.h"

libcompressor_Buffer libcompressor_compress(libcompressor_CompressionAlgorithm algo, libcompressor_Buffer input) {
    libcompressor_Buffer output;
    output.data = (char*)malloc(input.size + 1024);
    output.size = input.size + 1024;

    if (!output.data) { 
        output.size = 0;
        return output;
    }

    z_stream stream;
    memset(&stream, 0, sizeof(stream));

    if (deflateInit(&stream, Z_DEFAULT_COMPRESSION) != Z_OK) {
        free(output.data);
		output.data = NULL;
        output.size = 0;
        return output;
    }

    stream.next_in = (Bytef *)input.data;
    stream.avail_in = input.size;
    stream.next_out = (Bytef *)output.data;
    stream.avail_out = output.size;

    if (deflate(&stream, Z_FINISH) != Z_STREAM_END) {
        deflateEnd(&stream);
        free(output.data);
		output.data = NULL;
        output.size = 0;
        return output;
    }

    output.size = stream.total_out;
    deflateEnd(&stream);

    return output;
}

