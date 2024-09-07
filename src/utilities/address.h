#ifndef ADDRESS_H
#define ADDRESS_H

//#include "main.h"


#define PC_ADDRESS(rom_address, headered)		(((( (rom_address)&0x7F0000) >> 1) | ( (rom_address) & 0x7FFF)) + headered * 0x200)

#define ROM_ADDRESS(pc_address, headered)		(((((pc_address) - headered * 0x200) << 1) &0x7F0000) | (((pc_address) - headered * 0x200)&0x7FFF) | (0x8000))

class snes_address;
class pc_address;

class template_address
{
protected:
	unsigned int value = 0x000000;
public:
	template_address()								{ }
	template_address(const unsigned int &address)	{ value = address; }
	operator unsigned  int() const					{ return value; }	// TODO: check if EXPLICIT would be better.
	template_address& operator=( const unsigned int& address )
	{
		value = address;
		return *this;
	}

	template_address& operator++( const int )
	{
		value++;
		return *this;
	}

	template_address& operator--( const int )
	{
		value--;
		return *this;
	}

	template_address& operator+=( const unsigned int& added_address )
	{
		value += added_address;
		return *this;
	}

	template_address& operator-=( const unsigned int& subtracted_address )
	{
		value -= subtracted_address;
		return *this;
	}
};

/*template <typename convert_1, typename convert_2, unsigned int static_bits, unsigned int static_bits_value>
class template_snes_address : template_address
{
public:
	using template_address::template_address;
	using template_address::operator =;
	using template_address::operator unsigned int;

	bool is_valid() const {
		return ((this->value & static_bits) == static_bits_value);
	}

	template_snes_address(const convert_1& other);
	template_snes_address& operator=( const convert_1& other );

	template_snes_address(const convert_2& other);
	template_snes_address& operator=( const convert_2& other );


	snes_address& operator++( )
	{
		++value;
		return temp;
	}

	snes_address operator++( const int )
	{
		snes_address temp = *this;
		value++;
		return temp;
	}
};*/


class snes_address
{
protected:
	unsigned int value = 0x000000;
public:
	snes_address()									{ }
	snes_address(const unsigned int &address)		{ value = address; }
	operator unsigned  int() const					{ return value; }	// TODO: check if EXPLICIT would be better.
	snes_address& operator=( const unsigned int& address )
	{
		value = address;
		return *this;
	}

	snes_address& operator++( )
	{
		++value;
		return *this;
	}

	snes_address operator++( const int )
	{
		snes_address temp = *this;
		value++;
		return temp;
	}

	snes_address& operator--( )
	{
		--value;
		return *this;
	}

	snes_address operator--( const int )
	{
		snes_address temp = *this;
		value--;
		return temp;
	}

	snes_address& operator+=( const unsigned int& added_address )
	{
		value += added_address;
		return *this;
	}

	snes_address& operator-=( const unsigned int& subtracted_address )
	{
		value -= subtracted_address;
		return *this;
	}

	unsigned char low() const {
		return value & 0x0000FF;
	}

	unsigned char high() const {
		return (value & 0x00FF00) >> 8;
	}

	unsigned char bank() const {
		return (value & 0xFF0000) >> 16;
	}


	bool is_valid() const {
		return ((this->value & 0xFF000000) == 0x00000000);
	}

	bool is_valid_ram() const {
		return ((value & 0xFE0000) == 0x7E0000);
	}

	bool is_valid_rom() const {
		return ((value & 0x008000) == 0x008000);
	}

	pc_address to_pc_format(bool rom_has_header);
};


class pc_address
{
protected:
	unsigned int value = 0x000000;
public:
	pc_address()								{ }
	pc_address(const unsigned int &address)	{ value = address; }
	operator unsigned  int() const					{ return value; }	// TODO: check if EXPLICIT would be better.
	pc_address& operator=( const unsigned int& address )
	{
		value = address;
		return *this;
	}

	pc_address& operator++( )
	{
		++value;
		return *this;
	}

	pc_address operator++( const int )
	{
		pc_address temp = *this;
		value++;
		return temp;
	}

	pc_address& operator--( )
	{
		--value;
		return *this;
	}

	pc_address operator--( const int )
	{
		pc_address temp = *this;
		value--;
		return temp;
	}

	pc_address& operator+=( const unsigned int& added_address )
	{
		value += added_address;
		return *this;
	}

	pc_address& operator-=( const unsigned int& subtracted_address )
	{
		value -= subtracted_address;
		return *this;
	}

	bool is_valid() const {
		return ((value & 0xFF000000) == 0x00000000);
	}

	snes_address to_snes_format(bool rom_has_header);
};

#endif // ADDRESS_H
