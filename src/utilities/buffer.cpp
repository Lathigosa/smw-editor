#include "utilities/buffer.h"

buffer::buffer()
{
	buffer_exists = false;
	length = 0;
}

buffer::buffer(int size)
{
	buffer_data = new unsigned char[size];
	buffer_exists = true;
	length = size;
}

buffer::~buffer()
{
	if (buffer_exists == true) {
		delete[] buffer_data;
		buffer_exists = false;
	}
}

// "reset_size" will re-create a buffer and delete the old one if necessary.
void buffer::reset_size(int size)
{
	if (buffer_exists == true) {
		delete[] buffer_data;
		buffer_exists = false;
	}
	buffer_data = new unsigned char[size];
	length = size;
	buffer_exists = true;
}

void buffer::clear_buffer()
{
	if (buffer_exists == true) {
		int a;
		for (a=0;a<length;a++)
		{
			buffer_data[a] = 0;
		}
	}
}

// getBuffer allows you to edit the buffer. Use with caution!
unsigned char *buffer::getBuffer()
{
	return buffer_data;
}

// Copies an array into the buffer. Returns <false> if failed, <true> if succeeded.
bool buffer::import_buffer(const buffer &other_buffer)
{
	if (buffer_exists == false)
		return false;

	if (other_buffer.buffer_exists == false)
		return false;

	int a;
	if (length < other_buffer.length) {
		for (a=0; a<length; a++) {
			buffer_data[a] = other_buffer.buffer_data[a];
		}
		//qWarning() << "Copied buffer of length:" << length;
	} else {
		for (a=0; a<other_buffer.length; a++) {
			buffer_data[a] = other_buffer.buffer_data[a];
		}
		//qWarning() << "Copied buffer of other length:" << other_buffer.length;
	}
}

// Copies an array into the buffer. Returns <false> if failed, <true> if succeeded.
bool buffer::import_buffer_at(const buffer &other_buffer, unsigned int offset)
{
	if (buffer_exists == false)
		return false;

	if (other_buffer.buffer_exists == false)
		return false;

	int a;
	if (((int)length - (int)offset) < (int)other_buffer.length) {
		for (a=0; a<(length - offset); a++) {
			buffer_data[a + offset] = other_buffer.buffer_data[a];
		}
		//qWarning() << "Copied buffer of length:" << length << "with offset:" << offset;
	} else {
		for (a=0; a<(other_buffer.length); a++) {
			buffer_data[a + offset] = other_buffer.buffer_data[a];
			//qWarning() << "data [p" << QString("%1").arg(a + offset + 0x7E0000, 6, 16, QLatin1Char( '0' )) << "]:" << QString("%1").arg(buffer_data[a + offset], 2, 16, QLatin1Char( '0' ));
		}
		//qWarning() << "Copied buffer of other length:" << other_buffer.length << "with offset:" << offset;
	}
}

unsigned char buffer::get_byte(unsigned int index) const
{
	if(index >= length)
		return 0x00;

	return buffer_data[index];
}

bool buffer::set_byte(unsigned int index, unsigned char data)
{
	if(index >= length)
		return false;

	buffer_data[index] = data;

	return true;
}

int buffer::get_length() const
{
	return length;
}

// Assignment operators and copy constructors:
buffer::buffer( const buffer& other )
{
	length = other.get_length();

	//if(buffer_exists == true)
	//	delete[] buffer_data;
	buffer_data = new unsigned char[other.get_length()];
	buffer_exists = true;

	int a;
	for(a = 0;a < (other.get_length()); a++)
	{
		buffer_data[a] = other.buffer_data[a];
	}
}

buffer& buffer::operator=( const buffer& other )
{
	length = other.get_length();

	if(buffer_exists == true)
		delete[] buffer_data;
	buffer_data = new unsigned char[other.get_length()];
	buffer_exists = true;

	int a;
	for(a = 0;a < (other.get_length()); a++)
	{
		buffer_data[a] = other.buffer_data[a];
	}

	return *this;
}

buffer::buffer( const char* other )
{
	if(other == nullptr)
	{
		//if(buffer_exists == true)
		//	delete[] buffer_data;
		buffer_exists = false;
		length = 0;
		return;
	}

	length = 0;
	while(other[length] != 0x00)
	{
		length++;
	}

	length++;

	//if(buffer_exists == true)
	//	delete[] buffer_data;
	buffer_data = new unsigned char[length];
	buffer_exists = true;

	int a;
	for(a = 0;a < (length); a++)
	{
		buffer_data[a] = other[a];
	}
}

buffer& buffer::operator=( const char* other )
{
	if(other == nullptr)
	{
		if(buffer_exists == true)
			delete[] buffer_data;
		buffer_exists = false;
		length = 0;
		return *this;
	}

	length = 0;
	while(other[length] != 0x00)
	{
		length++;
	}

	length++;

	if(buffer_exists == true)
		delete[] buffer_data;
	buffer_data = new unsigned char[length];
	buffer_exists = true;

	int a;
	for(a = 0;a < (length); a++)
	{
		buffer_data[a] = other[a];
	}

	return *this;
}

element buffer::operator[](int index)
{
	if(index >= length)
		return element();

	return element(&buffer_data[index]);
}

unsigned char buffer::operator[](int index) const
{
	if(index >= length)
		return 0x00;

	return buffer_data[index];
}
