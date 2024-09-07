#ifndef COMPRESSION_H
#define COMPRESSION_H

#include "main.h"

// Decompresses any file with the LC_LZ2 format. Returns the decompressed buffer.
// max_size indicates how large you expect the decompressed file to be. Make it
//   as large or slightly larger than the file you expect, but not too large,
//   because it will create a buffer of that size (smaller is better).
// If the file is larger than max_size, the function will return <false>.
// If everything goes right, it returns <true>.
bool decompress_LC_LZ2(snes_address address, const SMW_ROM &rom, buffer &decompression_buffer, int max_size);



#endif // COMPRESSION_H
