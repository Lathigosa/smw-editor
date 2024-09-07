#ifndef DEFINITIONS_ROM_H
#define DEFINITIONS_ROM_H

// Converts the ROM address to a PC address by taking off 0x8000 from the address and adding a header if necessary.
// rom_address = int | unsigned int
// headered = bool

//#define PC_ADDRESS(rom_address, headered)		((((rom_address) & 0xFFFF)) + (0x8000 * (((rom_address) & 0xFF0000) >> 16)) - 0x8000 + (headered) * 0x0200)

#define ROM_ADDRESS_LOW(pc_address, headered)		((pc_address) & 0xFF)
#define ROM_ADDRESS_HIGH(pc_address, headered)		(((((pc_address) - (headered) * 0x0200)/0x0100)&0x7F)+0x80)
#define ROM_ADDRESS_BANK(pc_address, headered)		((((pc_address) - (headered) * 0x0200)/0x8000)&0xFF)

//((((((pc_address) - (headered) * 0x0200)&0xFF0000)+0x8000)/0x8000)&0xFF)

//#define ROM_ADDRESS_LOW(pc_address, headered)		(ROM_ADDRESS(pc_address, headered) & 0x0000FF)
//#define ROM_ADDRESS_HIGH(pc_address, headered)		(ROM_ADDRESS(pc_address, headered) & 0x00FF00)
//#define ROM_ADDRESS_BANK(pc_address, headered)		(ROM_ADDRESS(pc_address, headered) & 0xFF0000)

// TODO: Fix the PC_ADDRESS CONVERTER!!!

//int snespc(unsigned int addrlo, unsigned int addrhi, unsigned int bank)
//{
//return (addrlo&255)+(256*(addrhi&255))+(32768*bank)+header-32256;//65536-32256=33280
//}


#endif // DEFINITIONS_ROM_H
