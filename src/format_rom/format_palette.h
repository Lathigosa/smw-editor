#ifndef FORMAT_PALETTE_H
#define FORMAT_PALETTE_H

#include "main.h"

struct color_data {
	unsigned char byte_1;
	unsigned char byte_2;
	unsigned char r;
	unsigned char g;
	unsigned char b;

	void set_color(unsigned char red, unsigned char green, unsigned char blue);
	void get_color(unsigned char &red, unsigned char &green, unsigned char &blue);

	void set_color(unsigned char byte1, unsigned char byte2);
	void get_color(unsigned char &byte1, unsigned char &byte2);
};

class palette_data
{
public:
	palette_data();
	~palette_data();

	void setColor(unsigned char index, unsigned char red, unsigned char green, unsigned char blue);
	void getColor(unsigned char index, unsigned char &red, unsigned char &green, unsigned char &blue);

	void setColor(unsigned char index, unsigned char byte_1, unsigned char byte_2);
	void getColor(unsigned char index, unsigned char &byte_1, unsigned char &byte_2);

private:
	color_data data[256];	// The full palette.
};

#endif // FORMAT_PALETTE_H
