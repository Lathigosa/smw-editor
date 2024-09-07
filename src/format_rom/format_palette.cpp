#include "format_palette.h"




void color_data::set_color(unsigned char red, unsigned char green, unsigned char blue)
{
	r = red >> 3;
	g = green >> 3;
	b = blue >> 3;

	byte_1 = (b << 2 | g >> 3) & 0xFF;
	byte_2 = (g << 5 | r) & 0xFF;
}

void color_data::get_color(unsigned char &red, unsigned char &green, unsigned char &blue)
{
	red = r << 3;
	green = g << 3;
	blue = b << 3;
}

void color_data::set_color(unsigned char byte1, unsigned char byte2)
{
	b = ((byte1 & 0x7C) >> 2) << 3;
	g = (((byte1 & 0x03) << 3) | ((byte2 & 0xE0) >> 5)) << 3;
	r = ((byte2 & 0x1F)) << 3;

	byte_1 = byte1;
	byte_2 = byte2;
}

void color_data::get_color(unsigned char &byte1, unsigned char &byte2)
{
	byte1 = byte_1;
	byte2 = byte_2;
}







palette_data::palette_data()
{

}

palette_data::~palette_data()
{

}

void palette_data::setColor(unsigned char index, unsigned char red, unsigned char green, unsigned char blue)
{
	data[index].r = red >> 3;
	data[index].g = green >> 3;
	data[index].b = blue >> 3;

	data[index].byte_1 = (data[index].b << 2 | data[index].g >> 3) & 0xFF;
	data[index].byte_2 = (data[index].g << 5 | data[index].r) & 0xFF;
}

void palette_data::getColor(unsigned char index, unsigned char &red, unsigned char &green, unsigned char &blue)
{
	red = data[index].r << 3;
	green = data[index].g << 3;
	blue = data[index].b << 3;
}

void palette_data::setColor(unsigned char index, unsigned char byte_1, unsigned char byte_2)
{
	data[index].b = ((byte_1 & 0x7C) >> 2) << 3;
	data[index].g = (((byte_1 & 0x03) << 3) | ((byte_2 & 0xE0) >> 5)) << 3;
	data[index].r = ((byte_2 & 0x1F)) << 3;

	data[index].byte_1 = byte_1;
	data[index].byte_2 = byte_2;
}

void palette_data::getColor(unsigned char index, unsigned char &byte_1, unsigned char &byte_2)
{
	byte_1 = data[index].byte_1;
	byte_2 = data[index].byte_2;
}
