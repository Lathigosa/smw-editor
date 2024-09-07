#ifndef HEADER_TEST_H
#define HEADER_TEST_H

#include "main.h"

// test_header - Returns <true> if the ROM contains a header, <false> if not.
// *buffer: the ROM buffer (ROM->buffer).
bool test_header(unsigned char * buffer);

// test_header_genuine - Returns <true> if the ROM is an SMW ROM, <false> if not.
// *buffer: the ROM buffer (ROM->buffer).
bool test_header_genuine(unsigned char * buffer);

#endif // HEADER_TEST_H
