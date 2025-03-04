#include <gtest/gtest.h>

#include "libcompressor/libcompressor.hpp"

TEST(CompressionTest, emptytest) {
  libcompressor_Buffer input;
  input.data = const_cast<char*>("");
  input.size = 0;

  libcompressor_CompressionAlgorithm algo = libcompressor_Zlib;
  libcompressor_Buffer output = libcompressor_compress(algo, input);
  EXPECT_EQ(0, output.size);

  algo = libcompressor_Bzip;
  output = libcompressor_compress(algo, input);
  EXPECT_EQ(0, output.size);
}

TEST(CompressionTest, nulltest) {
  libcompressor_Buffer input;
  input.data = NULL;
  input.size = 0;

  libcompressor_CompressionAlgorithm algo = libcompressor_Zlib;
  libcompressor_Buffer output = libcompressor_compress(algo, input);
  EXPECT_EQ(0, output.size);

  algo = libcompressor_Bzip;
  output = libcompressor_compress(algo, input);
  EXPECT_EQ(0, output.size);
}

TEST(CompressionTest, zlibtest) {
  libcompressor_Buffer input;
  input.data = const_cast<char*>("test_string");
  input.size = strlen(input.data);

  unsigned char expected_output_data[] = {0x78, 0x9c, 0x2b, 0x49, 0x2d, 0x2e, 0x89, 0x2f, 0x2e, 0x29,
                                          0xca, 0xcc, 0x4b, 0x07, 0x00, 0x1c, 0x79, 0x04, 0xb7};
  libcompressor_Buffer expected_output;
  expected_output.data = reinterpret_cast<char*>(expected_output_data);
  expected_output.size = sizeof(expected_output_data);

  libcompressor_CompressionAlgorithm algo = libcompressor_Zlib;
  libcompressor_Buffer output = libcompressor_compress(algo, input);

  EXPECT_EQ(expected_output.size, output.size);
}

TEST(CompressionTest, bziptest) {
  libcompressor_Buffer input;
  input.data = const_cast<char*>("test_string");
  input.size = strlen(input.data);

  unsigned char expected_output_data[] = {0x42, 0x5a, 0x68, 0x31, 0x31, 0x41, 0x59, 0x26, 0x53, 0x59, 0x4a, 0x7c,
                                          0x69, 0x05, 0x00, 0x00, 0x04, 0x83, 0x80, 0x00, 0x00, 0x82, 0xa1, 0x1c,
                                          0x00, 0x20, 0x00, 0x22, 0x03, 0x68, 0x84, 0x30, 0x22, 0x50, 0xdf, 0x04,
                                          0x99, 0xe2, 0xee, 0x48, 0xa7, 0x0a, 0x12, 0x09, 0x4f, 0x8d, 0x20, 0xa0};

  libcompressor_Buffer expected_output;
  expected_output.data = reinterpret_cast<char*>(expected_output_data);
  expected_output.size = sizeof(expected_output_data);

  libcompressor_CompressionAlgorithm algo = libcompressor_Bzip;
  libcompressor_Buffer output = libcompressor_compress(algo, input);

  EXPECT_EQ(expected_output.size, output.size);
}
