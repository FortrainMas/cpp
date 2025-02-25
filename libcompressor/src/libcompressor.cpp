/**
 * @file libcompressor.cpp
 * @brief Implementation of compression functions using zlib and bzlib.
 *
 * This file contains the implementation of compression algorithms
 * using zlib and bzip2 for the libcompressor library.
 */

#include "libcompressor/libcompressor.hpp"

#include <stdlib.h>

#include <cstring>

#include "bzlib.h"
#include "zlib.h"

libcompressor_Buffer zlib_compress(libcompressor_Buffer input);
libcompressor_Buffer bzlib_compress(libcompressor_Buffer input);

/**
 * @brief Compresses the given buffer.
 * @param algo algorithm to use in compression.
 * @param input input libcompressor_buffer.
 * @return Compressed libcomressor_buffer.
 */
libcompressor_Buffer libcompressor_compress(libcompressor_CompressionAlgorithm algo, libcompressor_Buffer input) {
  if (input.size == 0) {
    libcompressor_Buffer lb = {const_cast<char*>(""), 0};
    return lb;
  }

  if (algo == libcompressor_Zlib) {
    return zlib_compress(input);
  } else {
    return bzlib_compress(input);
  }
}

/**
 * @brief Compressed the buffer using zlib.
 * @param input input buffer.
 * @return Compressed libcompressor_buffer.
 */
libcompressor_Buffer zlib_compress(libcompressor_Buffer input) {
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

  stream.next_in = (Bytef*)input.data;
  stream.avail_in = input.size;
  stream.next_out = (Bytef*)output.data;
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

/**
 * @brief Compressed the buffer using bzlib.
 * @param input input buffer.
 * @return Compressed libcompressor_buffer.
 */
libcompressor_Buffer bzlib_compress(libcompressor_Buffer input) {
  libcompressor_Buffer output;
  output.data = (char*)malloc(input.size + 1024);
  output.size = input.size + 1024;

  if (!output.data) {
    output.size = 0;
    return output;
  }

  bz_stream stream;
  memset(&stream, 0, sizeof(stream));
  if (BZ2_bzCompressInit(&stream, 1, 0, 0) != BZ_OK) {
    BZ2_bzDecompressEnd(&stream);
    free(output.data);
    output.data = NULL;
    output.size = 0;
    return output;
  }

  stream.next_in = input.data;
  stream.avail_in = input.size;
  stream.next_out = output.data;
  stream.avail_out = output.size;

  if (BZ2_bzCompress(&stream, BZ_FINISH) != BZ_STREAM_END) {
    BZ2_bzDecompressEnd(&stream);
    free(output.data);
    output.data = NULL;
    output.size = 0;
    return output;
  }

  output.size -= stream.avail_out;
  BZ2_bzDecompressEnd(&stream);

  return output;
}
