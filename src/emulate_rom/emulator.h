#ifndef EMULATE_OBJECT_H
#define EMULATE_OBJECT_H

//#include "main.h"
#include "utilities/buffer.h"
#include "utilities/address.h"

#include "load_rom/load_rom.h"

#include <QtOpenGL>

class level_object;
class level_header;
class level_map_data;
class object_block_queue;

enum opcode_mode {
	opcode_mode_invalid,
	absolute,
	indexed_x,
	indexed_y,
	indirect,
	indexed_indirect,
	indirect_indexed,
	indirect_long,
	indirect_indexed_long
};

enum struct emulation_error {
	emulator_success = 0,
	unused_opcode = 1,		// If an unprogrammed opcode is used, the emulator can't continue.
	emulator_stall = 2,		// If the emulator could not finish within the specified bounds.
	nullpointer_object,		// If the object points to a null address.
	object_table_altered,	// If the object table is altered.
	tilemap_out_of_bounds,	// When displaying a tilemap or level, the dimensions of the tilemap are too large for the given texture.
	map16_out_of_range,		// When displaying a level or map16 map, the requested map16 page does not exist.

};


class object_emulator
{
public:
	object_emulator(const SMW_ROM &loaded_rom, GLuint texture_gfx, GLuint texture_palette);
	~object_emulator();

	void upload_to_ram(buffer & data, unsigned int ram_address);
	void upload_to_ram(unsigned char data, unsigned int ram_address);
	unsigned char download_from_ram(unsigned int ram_address);
	buffer download_from_ram(unsigned int ram_address, unsigned int length);

	emulation_error emulate_gfx_animation(snes_address address, unsigned char frame_counter);
	emulation_error emulate_object_load(level_object &object, snes_address rom_address, level_header header, unsigned int object_index);
	void update_level_map( level_map_data &level_map );

	emulation_error emulate(snes_address address, unsigned char databank);

private:

	const SMW_ROM &rom;

	buffer ram_buffer = buffer(0x20000);

	buffer stack_buffer = buffer(245);
	unsigned int stack_pointer;

	// For object load:
	bool vertical_level;
	unsigned int current_object;
	object_block_queue * new_blocks;

	// For GFX animation:
	GLuint gfx_texture;
	GLuint palette_texture;
	bool cg_ram_write_ready;


	unsigned int get_address(unsigned char low, unsigned char high, unsigned char bank, opcode_mode mode);
	unsigned char get_ram_rom_byte(unsigned int rom_address);
	void set_ram_rom_byte(unsigned int rom_address, unsigned char data);

	bool push_stack(unsigned char data);
	unsigned char pull_stack();

	unsigned short a_register;
	unsigned short x_register;
	unsigned short y_register;

	unsigned char program_bank;
	unsigned short program_counter;
	unsigned char data_bank;

	unsigned char direct_page_low;
	unsigned char direct_page_high;
	unsigned char direct_page_bank;

	bool is_a_8_bit;
	bool is_x_y_8_bit;

	bool carry_flag;
	bool negative_flag;
	bool overflow_flag;
	bool zero_flag;
};

// This function emulates the loading of an object in the level map.
// This means that each opcode must be accounted for, hence
//   the length of the function.
// rom_address is the starting address at which the emulation begins,
//   and should be the address stored in one of the object address tables
//   in either the original ROM or in the ObjecTool hack
//   (0x0DA455 and 0x0DA10F for original ROM).
//error_emulation emulate_object_load(SMW_ROM &rom, level_object &object, unsigned int rom_address);

#endif // EMULATE_OBJECT_H
