#include "main.h"

// test_header - Returns <true> if the ROM contains a header, <false> if not.
// *buffer: the ROM buffer (ROM->buffer).
bool test_header(unsigned char * buffer) {
	unsigned char header_check[] = {0x78, 0x9C, 0x00, 0x42, 0x9C, 0x0C, 0x42, 0x9C};
	int a;
	for (a=0;a<8;a++)
	{
		if (buffer[a] != header_check[a]) {
			qWarning() << "This file has a header.";
			return true;
		}
	}
	return false;
}

// test_header_genuine - Returns <true> if the ROM is an SMW ROM, <false> if not.
// *buffer: the ROM buffer (ROM->buffer).
bool test_header_genuine(unsigned char * buffer) {
	bool has_header = test_header(buffer);

	unsigned char header_check[] = {0x78, 0x9C, 0x00, 0x42, 0x9C, 0x0C, 0x42, 0x9C};
	int a;
	if (has_header == true)	// If has_header is false, then the ROM is a genuine SMW rom, because test_header found the initial bytes.
	{
		for (a=0;a<8;a++)
		{
			if (buffer[a + 0x200] != header_check[a]) {
				qWarning() << "This file is not smw.";
				return false;
			}
		}
	}
	return true;
}
