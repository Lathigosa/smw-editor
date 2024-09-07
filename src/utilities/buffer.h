#ifndef BUFFER_H
#define BUFFER_H

//#include "main.h"

struct element {
	unsigned char * data;

	element(unsigned char *a) : data(a) {}
	element() : data(nullptr) {}
	operator unsigned char() const {
		if(data != nullptr)
			return *data;
		else
			return 0x00;
	}
	unsigned char operator=(const unsigned char value) {
		if(data != nullptr)
			*data = value;

		return value;
	}
};

class buffer {
public:
	buffer();
	buffer(int size);
	~buffer();
	void reset_size(int size);
	void clear_buffer();
	bool import_buffer(const buffer &other_buffer);
	bool import_buffer_at(const buffer &other_buffer, unsigned int offset);
	unsigned char *getBuffer();


	int get_length() const;

	unsigned char get_byte(unsigned int index) const;
	bool set_byte(unsigned int index, unsigned char data);

	buffer( const buffer& other );
	buffer& operator=( const buffer& other );

	buffer(const char *other );
	buffer& operator=( const char* other );
private:
	int length;
	bool buffer_exists;

	unsigned char * buffer_data;

public:
	element operator[](int index);
	unsigned char operator[](int index) const;
};

#endif // BUFFER_H
