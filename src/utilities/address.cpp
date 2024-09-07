#include "address.h"



/*template_address::template_address()
{

}

template_address::template_address(const unsigned int& address)
{
	value = address;
}

template_address& template_address::operator=( const unsigned int& address )
{
	value = address;
	return *this;
}

template_address& template_address::operator++( const int )
{
	value++;
	return *this;
}

template_address& template_address::operator--( const int )
{
	value--;
	return *this;
}

template_address& template_address::operator+=( const unsigned int& added_address )
{
	value += added_address;
	return *this;
}

template_address& template_address::operator-=( const unsigned int& subtracted_address )
{
	value -= subtracted_address;
	return *this;
}*/

/* template <typename c1, typename c2, unsigned int sb, unsigned int sbv>
template_snes_address<c1, c2, sb, sbv>::snes_address(const c1& other)
{
	value = (unsigned int)other;
}

template <typename c1, typename c2, unsigned int sb, unsigned int sbv>
template_snes_address& snes_address::operator=( const c1& other ) {
	value = (unsigned int)other;
	return *this;
}

template <typename c1, typename c2, unsigned int sb, unsigned int sbv>
template_snes_address<c1, c2, sb, sbv>::snes_address(const c2& other)
{
	value = (unsigned int)other;
}

template <typename c1, typename c2, unsigned int sb, unsigned int sbv>
template_snes_address& snes_address::operator=( const c2& other ) {
	value = (unsigned int)other;
	return *this;
} */

//snes_address::snes_address()
//{

//}

pc_address snes_address::to_pc_format(bool rom_has_header)
{
	if(!this->is_valid())
		return 0xFFFFFFFF;

	return (unsigned int) PC_ADDRESS(value, rom_has_header);
}


//pc_address::pc_address()
//{

//}

snes_address pc_address::to_snes_format(bool rom_has_header)
{
	if(!this->is_valid())
		return 0x00000000;

	return (unsigned int) ROM_ADDRESS(value, rom_has_header);
}
