//! Supported compression algorithms.
enum libcompressor_CompressionAlgorithm {
    libcompressor_Zlib, /**< Use zlib for compression. */
    libcompressor_Bzip  /**< Use bzip2 for compression. */
};

/**
 * @brief Represents a buffer for compression or decompression.
 */
struct libcompressor_Buffer {
    char * data; /**< Pointer to the buffer data. */
    int size;    /**< Size of the buffer in bytes. */
};

/**
 * @brief Compresses the given buffer.
 * @param algo Algorithm to use for compression.
 * @param input Input buffer to be compressed.
 * @return Compressed buffer.
 */
libcompressor_Buffer libcompressor_compress(libcompressor_CompressionAlgorithm algo, libcompressor_Buffer input);
