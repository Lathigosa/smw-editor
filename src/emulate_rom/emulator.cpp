#include "main.h"

unsigned short ror_short(unsigned short value, int shift)	{ return (value >> shift) | (value << (sizeof(value) * CHAR_BIT - shift)); }
unsigned short rol_short(unsigned short value, int shift)	{ return (value << shift) | (value >> (sizeof(value) * CHAR_BIT - shift)); }
unsigned char ror_byte(unsigned char value, int shift)		{ return (value >> shift) | (value << (sizeof(value) * CHAR_BIT - shift)); }
unsigned char rol_byte(unsigned char value, int shift)		{ return (value << shift) | (value >> (sizeof(value) * CHAR_BIT - shift)); }

unsigned short ror(unsigned short value, int shift, bool is_a_8_bit) {
	if (is_a_8_bit == false)
		return ror_short(value, shift);
	else
		return ror_byte(value & 0xFF, shift);
}

unsigned short rol(unsigned short value, int shift, bool is_a_8_bit) {
	if (is_a_8_bit == false)
		return rol_short(value, shift);
	else
		return rol_byte(value & 0xFF, shift);
}

void object_emulator::upload_to_ram(buffer & data, unsigned int ram_address)
{
	if(ram_address < 0x7E0000 || ram_address > 0x7FFFFF)
		return;

	// Copy the data buffer to the ram buffer.
	ram_buffer.import_buffer_at(data, ram_address - 0x7E0000);
}

void object_emulator::upload_to_ram(unsigned char data, unsigned int ram_address)
{
	if(ram_address < 0x7E0000 || ram_address > 0x7FFFFF)
		return;

	ram_buffer.set_byte(ram_address - 0x7E0000, data);
}

unsigned char object_emulator::download_from_ram(unsigned int ram_address)
{
	if(ram_address < 0x7E0000 || ram_address > 0x7FFFFF)
		return 0x00;

	return ram_buffer.get_byte(ram_address - 0x7E0000);
}

buffer object_emulator::download_from_ram(unsigned int ram_address, unsigned int length)
{
	if(ram_address < 0x7E0000 || ram_address > 0x7FFFFF)
		return 0x00;

	buffer output(length);

	for(unsigned int i=0; i<length; i++)
	{
		output.set_byte(i, ram_buffer.get_byte(ram_address - 0x7E0000 + i));
	}

	return output;
}

unsigned char object_emulator::get_ram_rom_byte(unsigned int rom_address)
{


	if(rom_address < 0x7E0000 || rom_address > 0x7FFFFF)
	{
		if((rom_address & 0xFFFF) < 0x8000)
			return ram_buffer.get_byte(rom_address & 0xFFFF);

		return rom.get_byte_pc(PC_ADDRESS(rom_address, rom.has_header));
	}

	return ram_buffer.get_byte(rom_address - 0x7E0000);
}

void object_emulator::set_ram_rom_byte(unsigned int rom_address, unsigned char data)
{
	// Test if the address to be set is a map16 block:
	if((rom_address & 0xFFFF) >= 0xC800)
	{
		if(vertical_level == false)		// If the level is a HORIZONTAL level.
		{
			// Add block (horizontal level)
			unsigned int position_data = (rom_address & 0x00FFFF) - 0xC800;
			unsigned int screen_number = position_data / 0x1B0;											// Gets the screen number by dividing with screen size (answer is truncated to zero, so it will report the exact screen number).
			unsigned int subscreen_y = (position_data - (screen_number * 0x1B0)) / 16;					// Gets the y position within a screen by subtracting the previous screens and dividing by x_size (16).
			unsigned int subscreen_x = (position_data - (screen_number * 0x1B0) - (subscreen_y * 16));	// Gets the x position within a screen by subtracting the previous answers (giving you the remain, thus the x).

			unsigned int position_x = screen_number * 0x10 + subscreen_x;

			if((rom_address & 0xFF0000) == 0x7E0000)
				new_blocks->addItemLow(data, position_x, subscreen_y, current_object);


			if((rom_address & 0xFF0000) == 0x7F0000)
				new_blocks->addItemHigh(data, position_x, subscreen_y, current_object);

		}
		else							// If the level is a VERTICAL level.
		{
			// Add block (vertical level)
			unsigned int position_data = (rom_address & 0x00FFFF) - 0xC800;
			unsigned int screen_number = (position_data & 0x3E00) >> 9;									// Gets the screen number by dividing with screen size (answer is truncated to zero, so it will report the exact screen number).
			unsigned int subscreen_y = (position_data & 0xF0) >> 4;										// Gets the y position within a screen by subtracting the previous screens and dividing by x_size (16).
			unsigned int subscreen_x = (position_data & 0x0F) | ((position_data & 0x100) >> 4);			// Gets the x position within a screen by subtracting the previous answers (giving you the remain, thus the x).

			unsigned int position_y = screen_number * 0x10 + subscreen_y;

			if((rom_address & 0xFF0000) == 0x7E0000)
				new_blocks->addItemLow(data, subscreen_x, position_y, current_object);

			if((rom_address & 0xFF0000) == 0x7F0000)
				new_blocks->addItemHigh(data, subscreen_x, position_y, current_object);

		}
	}

	// Make sure that the emulator does not try to write to ROM:
	if(rom_address < 0x7E0000 || rom_address > 0x7FFFFF)
	{
		if((rom_address & 0xFFFF) >= 0x8000) {
			qWarning() << "ERROR: Attempt by emulator to change ROM!";
			return;
		}
		// Correct for direct page access:
		rom_address = 0x7E0000 + (rom_address & 0xFFFF);

	}

	// This is for tile gfx animation:
	// If two bytes are written to the RAM addresses containing RAM sources for the gfx animation,
	// write to texture. DMA CHANNEL #2 ENABLE
	if(rom_address == 0x7E420B && data == 0x04)
	{
		unsigned char address_low = ram_buffer.get_byte(0x4322);
		unsigned char address_high = ram_buffer.get_byte(0x4323);
		unsigned char address_bank = ram_buffer.get_byte(0x4324);
		unsigned int source_ram_address = get_address(address_low, address_high, address_bank, absolute);

		unsigned char transfer_size_low = ram_buffer.get_byte(0x4325);
		unsigned char transfer_size_high = ram_buffer.get_byte(0x4326);
		unsigned int transfer_size = (transfer_size_low) | (transfer_size_high << 8);

		unsigned char VRAM_address_low = ram_buffer.get_byte(0x2116);
		unsigned char VRAM_address_high = ram_buffer.get_byte(0x2117);
		unsigned int VRAM_address = ((VRAM_address_low) | (VRAM_address_high << 8)) * 2;	// The VRAM address has to be multiplied by two here, because the VRAM address given is a word address, while we need a byte address.

		// Calculate the x-offset and y-offset inside the texture:
		unsigned int texture_tile_offset = VRAM_address / 32;					// Every 32 bytes make up one tile.
		unsigned int tile_y_offset = texture_tile_offset / 16;					// Every horizontal row contains 16 tiles.
		unsigned int tile_x_offset = texture_tile_offset - tile_y_offset * 16;	// The x offset is the remainder of the tiles.

		// Calculate the amount of tiles that will be placed:
		unsigned int tiles_x = transfer_size / 32;

		// Make sure that the row fits in the texture. If not, quit with a warning:
		if((tiles_x + tile_x_offset) > 16) {
			qWarning() << "ERROR: The tiles are too large:" << tiles_x << tile_x_offset << "VRAM_address" << QString("%1").arg(VRAM_address, 4, 16, QLatin1Char( '0' ));
			return;
		}

		// Correct for the fact that the OpenGL texture has the tiles from page 4, 5, 6 and 7 on the right side:
		if(tile_y_offset >= 32)
		{
			tile_y_offset -= 32;
			tile_x_offset += 16;
		}

		// Upload the actual data:
		if(source_ram_address < 0x7E0000 || source_ram_address > 0x7FFFFF) {
			// The emulator tries to access the ROM. This is allowed for DMA, but currently unsupported.
			qWarning() << "ERROR: Attempt by emulator to DMA from ROM. This is currently unsupported.";

		} else {
			load_gfx_to_texture(ram_buffer, source_ram_address - 0x7E0000, transfer_size, BPP_4, gfx_texture, tiles_x, 1, tile_x_offset, tile_y_offset);
		}

		return;
	}

	// This is for tile gfx animation:
	// If two bytes are written to the RAM addresses containing RAM sources for the gfx animation,
	// write to texture. DMA CHANNEL #2 ENABLE
	if(rom_address == 0x7E2122)
	{
		if(cg_ram_write_ready == false)
		{
			// Write the current data value to RAM, so the next time it can be read again.
			cg_ram_write_ready = true;
		} else {
			// The previous time, the data was stored to RAM 0x7E2122, so now we can use it.
			unsigned char byte_high = data;
			unsigned char byte_low = ram_buffer.get_byte(0x2122);

			unsigned char colors[3] = {
				((byte_low & 0x1F)) << 3,
				(((byte_high & 0x03) << 3) | ((byte_low & 0xE0) >> 5)) << 3,
				((byte_high & 0x7C) >> 2) << 3
			};

			// Get the address for the write:
			unsigned char palette_address = ram_buffer.get_byte(0x2121);
			unsigned char palette_x = palette_address & 0x0F;
			unsigned char palette_y = (palette_address & 0xF0) >> 4;

			// Upload the texture:
			glBindTexture(GL_TEXTURE_2D, palette_texture);

			glTexSubImage2D(GL_TEXTURE_2D,
				0,
				palette_x,
				palette_y,
				1,
				1,
				GL_RGB,
				GL_UNSIGNED_BYTE,
				reinterpret_cast<void*>(colors));

			cg_ram_write_ready = false;

			return;
		}
	}


	ram_buffer.set_byte(rom_address - 0x7E0000, data);
}

// This function submits the newly rendered object to the level map.
// This function should be called each time a new object has been emulated.
void object_emulator::update_level_map( level_map_data &level_map )
{
	//TODO: add history queue (undo queue).
	new_blocks->applyQueue(level_map);
}

object_emulator::object_emulator(const SMW_ROM &loaded_rom, GLuint texture_gfx, GLuint texture_palette) : rom(loaded_rom)
{
	//ram_buffer = new buffer(0x20000);
	ram_buffer.clear_buffer();
	unsigned int a;
	for (a=0xC800; a<0x10000; a++) {
		ram_buffer.set_byte(a, 0x25);
	}


	//stack_buffer = new buffer(245);

	new_blocks = new object_block_queue;

	gfx_texture = texture_gfx;
	palette_texture = texture_palette;

	cg_ram_write_ready = false;
}

object_emulator::~object_emulator()
{
	//delete ram_buffer;
	//delete stack_buffer;
	delete new_blocks;
}

bool object_emulator::push_stack(unsigned char data)
{
	if (stack_pointer == 245)
		return false;

	stack_buffer.set_byte(stack_pointer, data);
	stack_pointer++;

	return true;
}

unsigned char object_emulator::pull_stack()
{
	stack_pointer--;
	return stack_buffer.get_byte(stack_pointer);
}

unsigned int object_emulator::get_address(unsigned char low, unsigned char high, unsigned char bank, opcode_mode mode)
{
	unsigned short bit_mask_x_y;

	if(is_x_y_8_bit == true)
		bit_mask_x_y = 0x00FF;
	else
		bit_mask_x_y = 0xFFFF;

	switch (mode) {
	case absolute:						// address = address
		return ((low) | (high << 8) | (bank << 16));
		break;
	case indexed_x:						// address = address + x
		return ((low) | (high << 8) | (bank << 16)) + (x_register & bit_mask_x_y);
		break;
	case indexed_y:						// address = address + y
		return ((low) | (high << 8) | (bank << 16)) + (y_register & bit_mask_x_y);
		break;
	case indirect:						// address = (int16)rom[address] | (program_bank << 16)		//TODO: Should this be program_bank or data_bank?
		return get_ram_rom_byte(  (low) | (high << 8) | (bank << 16)      ) |
				(get_ram_rom_byte( ((low) | (high << 8) | (bank << 16)) + 1 ) << 8) |
				(program_bank << 16);
		break;
	case indexed_indirect:				// address = (int16)rom[address + x] | (program_bank << 16)
		return get_ram_rom_byte( ((low) | (high << 8) | (bank << 16)) + (x_register & bit_mask_x_y)     ) |
				(get_ram_rom_byte( ((low) | (high << 8) | (bank << 16)) + (x_register & bit_mask_x_y) + 1 ) << 8) |
				(program_bank << 16);
		break;
	case indirect_indexed:				// address = (int16)rom[address] + y | (program_bank << 16)
		return ((get_ram_rom_byte( ((low) | (high << 8) | (bank << 16))     ) |
				(get_ram_rom_byte( ((low) | (high << 8) | (bank << 16)) + 1 ) << 8) |
				(program_bank << 16))) + (y_register & bit_mask_x_y);
		break;
	case indirect_long:					// address = (int24)rom[address]
		return get_ram_rom_byte( ((low) | (high << 8) | (bank << 16))     ) |
				(get_ram_rom_byte( ((low) | (high << 8) | (bank << 16)) + 1 ) << 8) |
				(get_ram_rom_byte( ((low) | (high << 8) | (bank << 16)) + 2 ) << 16);
	case indirect_indexed_long:			// address = (int24)rom[address] + y
		return ((get_ram_rom_byte( ((low) | (high << 8) | (bank << 16))     )) |
				(get_ram_rom_byte( ((low) | (high << 8) | (bank << 16)) + 1 ) << 8) |
				(get_ram_rom_byte( ((low) | (high << 8) | (bank << 16)) + 2 ) << 16))
				+ (y_register & bit_mask_x_y);
		break;
	default:
		return 0;
		break;
	}
}

// This function emulates the animation of the gfx tiles.
// rom_address is the starting address at which the emulation begins,
//   and should be the address "05BB39" in the original ROM.
// List of unsupported opcodes:
//   Stack relative (impossible to predict): 63, 23, C3, 43, A3, 03, E3, 83, 73, 33, D3, 53, B3, 13, F3, 93 (stack relative)
emulation_error object_emulator::emulate_gfx_animation(snes_address address, unsigned char frame_counter)
{


	stack_pointer = 0;										// Reset the stack pointer.
	data_bank = 0x00;										// Reset the data bank (00 is the data bank the emulator will be on when uploading GFX, and it is irrelevant for the updating routine).

	ram_buffer.set_byte(0x14, frame_counter);	// Upload the frame counter to emulator RAM.
	//ram_buffer.setByte(0x1931, 3);
	ram_buffer.set_byte(0x14AD, 0);				// Set the pow switch off.
	ram_buffer.set_byte(0x14AE, 0);				// Set the pow switch off.
	ram_buffer.set_byte(0x14AF, 0);				// Set the ON|OFF switch to ON.

	// Initialize registers (replicating the ASM code before the gfx-animation-ASM runs):


	return emulate(address, 0x00);
}

// This function emulates the loading of an object in the level map.
// This means that each opcode must be accounted for, hence the
//   length of the function.
// rom_address is the starting address at which the emulation begins,
//   and should be the address stored in one of the object address tables
//   in either the original ROM or in the ObjecTool hack
//   (0x0DA455 and 0x0DA10F for original ROM).
// List of unsupported opcodes:
//   Stack relative (impossible to predict): 63, 23, C3, 43, A3, 03, E3, 83, 73, 33, D3, 53, B3, 13, F3, 93 (stack relative)
emulation_error object_emulator::emulate_object_load( level_object &object, snes_address rom_address, level_header header, unsigned int object_index)
{
	vertical_level = header.is_vertical_level;
	ram_buffer.set_byte(0x5B, header.is_vertical_level);		// Low

	stack_pointer = 0;
	data_bank = 0x7E;

	current_object = object_index;

	// Set the tileset setting:
	ram_buffer.set_byte(0x1931, header.tile_set);

	// Get the screen index, subscreen x and subscreen y of the current object:
	// TODO: calculate for vertical levels as well:
	// Low byte map:
	unsigned int map_ram_address;
	if (vertical_level == false) {
		map_ram_address = (object.screen_number * 0x01B0) + 0x7EC800;

		// If Y is in bottom subscreen:
		if ((object.position_Y & 0x10) == 0x10)
			map_ram_address += 0x100;
	} else {
		map_ram_address = (object.screen_number * 0x0200) + 0x7EC800;

		// If X is in right subscreen:
		if ((object.position_X & 0x10) == 0x10)
			map_ram_address += 0x100;
	}
	ram_buffer.set_byte(0x6B, (map_ram_address) & 0xFF);			// Low
	ram_buffer.set_byte(0x6C, (map_ram_address >> 8) & 0xFF);	// High
	ram_buffer.set_byte(0x6D, (map_ram_address >> 16) & 0xFF);	// Bank

	// High byte map:
	if (vertical_level == false) {
		map_ram_address = (object.screen_number * 0x01B0) + 0x7FC800;

		// If Y is in bottom subscreen:
		if (object.position_Y > 0x0F)
			map_ram_address += 0x100;
	} else {
		map_ram_address = (object.screen_number * 0x0200) + 0x7FC800;

		// If X is in right subscreen:
		if (object.position_X > 0x0F)
			map_ram_address += 0x100;
	}

	ram_buffer.set_byte(0x6E, (map_ram_address) & 0xFF);			// Low
	ram_buffer.set_byte(0x6F, (map_ram_address >> 8) & 0xFF);	// High
	ram_buffer.set_byte(0x70, (map_ram_address >> 16) & 0xFF);	// Bank

	// Set general settings of object:
	ram_buffer.set_byte(0x5A, object.object_number);
	ram_buffer.set_byte(0x59, object.settings_byte_1);
	ram_buffer.set_byte(0x58, object.settings_byte_3);

	//if (vertical_level == false) {
	ram_buffer.set_byte(0x57, (((object.position_Y) & 0xF) << 4) + (object.position_X & 0x0F));
	//} else {
	//	ram_buffer.setByte(0x57, (((object.position_X) & 0xF) << 4) + (object.position_Y & 0x0F));
	//}

	// TODO: Change the next two lines of code to include support for objects which require more data:

	// Set the level data address pointer to 0x0000.
	ram_buffer.set_byte(0x65, 0x00);
	ram_buffer.set_byte(0x66, 0x00);


	// Initialize registers (replicating the ASM code which runs before the object-ASM):

	ram_buffer.set_byte(0x1928, object.screen_number);
	ram_buffer.set_byte(0x1BA1, object.screen_number);
	if (object.object_number == 0x00)
	{
		x_register = object.settings_byte_1;
		a_register = object.settings_byte_1;
	} else {
		x_register = object.object_number - 1;
		a_register = object.object_number - 1;
	}

	return emulate(rom_address, 0x7E);
}

#define OPCODE_MODE_CONST					dp_data = (rom.get_byte_pc(current_pc_address + 1)) | (rom.get_byte_pc(current_pc_address + 2) << 8); \
											break

#define OPCODE_MODE_DIRECT_PAGE(mode)		dp_address = get_address(rom.get_byte_pc(current_pc_address + 1) + direct_page_low, direct_page_high, 0x00, mode); \
											dp_data = (get_ram_rom_byte(dp_address)) | (get_ram_rom_byte(dp_address + 1) << 8); \
											break																											// TODO: Add direct_page_low

#define OPCODE_MODE_NORMAL(mode)			dp_address = get_address(rom.get_byte_pc(current_pc_address + 1), rom.get_byte_pc(current_pc_address + 2), data_bank, mode); \
											dp_data = (get_ram_rom_byte(dp_address)) | (get_ram_rom_byte(dp_address + 1) << 8); \
											break

#define OPCODE_MODE_LONG(mode)				dp_address = get_address(rom.get_byte_pc(current_pc_address + 1), rom.get_byte_pc(current_pc_address + 2), rom.get_byte_pc(current_pc_address + 3), mode); \
											dp_data = (get_ram_rom_byte(dp_address)) | (get_ram_rom_byte(dp_address + 1) << 8); \
											break

#define OPCODE_MODE_PROGRAM(mode)			dp_address = get_address(rom.get_byte_pc(current_pc_address + 1), rom.get_byte_pc(current_pc_address + 2), program_bank, mode); \
											dp_data = (get_ram_rom_byte(dp_address)) | (get_ram_rom_byte(dp_address + 1) << 8); \
											break

#define NEXT(number)						current_pc_address += number

#define NEXT_CONST							current_pc_address += 2; \
											if(is_a_8_bit == false) \
												current_pc_address++;	// If the #const is two bytes long, add one extra to offset:

#define NEXT_CONST_XY						current_pc_address += 2; \
											if(is_x_y_8_bit == false) \
												current_pc_address++;	// If the #const is two bytes long, add one extra to offset:

#define OPERATOR_ORA						a_register = a_register | (dp_data & bit_mask); \
											zero_flag = !(a_register & bit_mask); \
											negative_flag = a_register & negative_bit

#define OPERATOR_ASL_A						carry_flag = a_register & negative_bit; \
											a_register = (a_register & (0xFF00 * is_a_8_bit)) | ((a_register << 1) & bit_mask); \
											zero_flag = !(a_register & bit_mask); \
											negative_flag = a_register & negative_bit

#define OPERATOR_ASL_ADDR					carry_flag = dp_data & negative_bit; \
											dp_data = (dp_data << 1) & bit_mask; \
											set_ram_rom_byte(dp_address, dp_data & 0xFF); \
											if (is_a_8_bit == false) \
												set_ram_rom_byte(dp_address + 1, (dp_data << 8) & 0xFF); \
											zero_flag = !(dp_data & bit_mask); \
											negative_flag = dp_data & negative_bit;

#define OPERATOR_LSR_ADDR					carry_flag = dp_data & 0x01; \
											dp_data = ((dp_data & bit_mask) >> 1); \
											set_ram_rom_byte(dp_address, dp_data & 0xFF); \
											if (is_a_8_bit == false) \
												set_ram_rom_byte(dp_address + 1, (dp_data << 8) & 0xFF); \
											zero_flag = !(dp_data & bit_mask); \
											negative_flag = dp_data & negative_bit;

#define OPERATOR_AND						a_register = (a_register & (0xFF00 * is_a_8_bit)) | (a_register & (dp_data & bit_mask)); \
											zero_flag = !(a_register & bit_mask); \
											negative_flag = a_register & negative_bit

#define OPERATOR_INC_A						a_register++; \
											zero_flag = !(a_register & bit_mask); \
											negative_flag = a_register & negative_bit

#define OPERATOR_DEC_A						a_register--; \
											zero_flag = !(a_register & bit_mask); \
											negative_flag = a_register & negative_bit

#define OPERATOR_EOR						a_register = (a_register ^ (dp_data & bit_mask)); \
											zero_flag = !(a_register & bit_mask); \
											negative_flag = a_register & negative_bit

#define OPERATOR_LDA						a_register = (a_register & (0xFF00 * is_a_8_bit)) | (dp_data & bit_mask); \
											zero_flag = !(a_register & bit_mask); \
											negative_flag = a_register & negative_bit;

#define OPERATOR_LDX						x_register = (x_register & (0xFF00 * is_x_y_8_bit)) | (dp_data & bit_mask_x_y); \
											zero_flag = !(x_register & bit_mask_x_y); \
											negative_flag = x_register & negative_bit_x_y;

#define OPERATOR_LDY						y_register = (y_register & (0xFF00 * is_x_y_8_bit)) | (dp_data & bit_mask_x_y); \
											zero_flag = !(y_register & bit_mask_x_y); \
											negative_flag = y_register & negative_bit_x_y;

#define OPERATOR_STA						set_ram_rom_byte(dp_address, a_register & 0xFF); \
											if (is_a_8_bit == false) \
												set_ram_rom_byte(dp_address + 1, (a_register >> 8) & 0xFF);

#define OPERATOR_CMP						{ unsigned short temp_register; \
											temp_register = (a_register & bit_mask) - (dp_data & bit_mask); \
											zero_flag = !(temp_register & bit_mask); \
											negative_flag = temp_register & negative_bit; \
											carry_flag = ((dp_data & bit_mask) <= (a_register & bit_mask)); }	// TODO: is this comparison correct?

#define OPERATOR_CPY						{ unsigned short temp_register; \
											temp_register = (y_register & bit_mask_x_y) - (dp_data & bit_mask_x_y); \
											zero_flag = !(temp_register & bit_mask_x_y); \
											negative_flag = temp_register & negative_bit_x_y; \
											carry_flag = ((dp_data & bit_mask_x_y) <= (y_register & bit_mask_x_y)); }	// TODO: is this comparison correct?

#define OPERATOR_CPX						{ unsigned short temp_register; \
											temp_register = (x_register & bit_mask_x_y) - (dp_data & bit_mask_x_y); \
											zero_flag = !(temp_register & bit_mask_x_y); \
											negative_flag = temp_register & negative_bit_x_y; \
											carry_flag = ((dp_data & bit_mask_x_y) <= (x_register & bit_mask_x_y)); }	// TODO: is this comparison correct?



#define OPERATOR_SBC						{bool temp_negative_flag = a_register & negative_bit; \
											a_register = (a_register & (0xFF00 * is_a_8_bit)) | ((a_register & bit_mask) - (dp_data & bit_mask) - 1 + carry_flag); \
											zero_flag = !(a_register & bit_mask); \
											negative_flag = a_register & negative_bit; \
											overflow_flag = (temp_negative_flag == true && negative_flag == false); \
											carry_flag = !((a_register & bit_mask) > (bit_mask - (dp_data & bit_mask))); }

#define OPERATOR_ADC						{bool temp_negative_flag = a_register & negative_bit; \
											a_register = (a_register & (0xFF00 * is_a_8_bit)) | ((a_register + (dp_data & bit_mask) + carry_flag) & bit_mask); \
											zero_flag = !(a_register & bit_mask); \
											negative_flag = a_register & negative_bit; \
											overflow_flag = (temp_negative_flag == false && negative_flag == true); \
											carry_flag = ((a_register & bit_mask) < (dp_data & bit_mask)); }

#define OPERATOR_INC_ADDR					dp_data += 1; \
											set_ram_rom_byte(dp_address, dp_data & 0xFF); \
											if (is_a_8_bit == false) \
												set_ram_rom_byte(dp_address + 1, (dp_data >> 8) & 0xFF); \
											zero_flag = !(dp_data & bit_mask); \
											negative_flag = dp_data & negative_bit;

#define OPERATOR_DEC_ADDR					dp_data -= 1; \
											set_ram_rom_byte(dp_address, dp_data & 0xFF); \
											if (is_a_8_bit == false) \
												set_ram_rom_byte(dp_address + 1, (dp_data >> 8) & 0xFF); \
											zero_flag = !(dp_data & bit_mask); \
											negative_flag = dp_data & negative_bit;

#define OPERATOR_BIT						{ unsigned short temp_register = (a_register & (0xFF00 * is_a_8_bit)) | (a_register & (dp_data & bit_mask)); \
											zero_flag = !(temp_register & bit_mask); \
											negative_flag = temp_register & negative_bit; }

#define OPERATOR_ROL						carry_flag = dp_data & negative_bit; \
											dp_data = rol(dp_data, 1, is_a_8_bit); \
											set_ram_rom_byte(dp_address, dp_data & 0xFF); \
											if (is_a_8_bit == false) \
												set_ram_rom_byte(dp_address + 1, (dp_data << 8) & 0xFF); \
											zero_flag = !(dp_data & bit_mask); \
											negative_flag = dp_data & negative_bit;

#define OPERATOR_ROL_A						carry_flag = a_register & negative_bit; \
											a_register = rol(a_register, 1, is_a_8_bit); \
											zero_flag = !(a_register & bit_mask); \
											negative_flag = a_register & negative_bit;

#define OPERATOR_ROR_ADDR					carry_flag = dp_data & 0x01; \
											dp_data = ror(dp_data, 1, is_a_8_bit); \
											set_ram_rom_byte(dp_address, dp_data & 0xFF); \
											if (is_a_8_bit == false) \
												set_ram_rom_byte(dp_address + 1, (dp_data << 8) & 0xFF); \
											zero_flag = !(dp_data & bit_mask); \
											negative_flag = dp_data & negative_bit;

#define OPERATOR_STY						set_ram_rom_byte(dp_address, y_register & 0xFF); \
											if (is_x_y_8_bit == false) \
												set_ram_rom_byte(dp_address + 1, (y_register >> 8) & 0xFF);

#define OPERATOR_STX						set_ram_rom_byte(dp_address, x_register & 0xFF); \
											if (is_x_y_8_bit == false) \
												set_ram_rom_byte(dp_address + 1, (x_register >> 8) & 0xFF);

#define OPERATOR_STZ						set_ram_rom_byte(dp_address, 0x00); \
											if (is_a_8_bit == false) \
												set_ram_rom_byte(dp_address + 1, 0x00);

#define OPERATOR_BRA						{ unsigned char relative_address_low = rom.get_byte_pc(current_pc_address + 1); \
											unsigned char flipped_relative_address = ~relative_address_low + 1; \
											if((relative_address_low & 0x80) == 0x80)	/* If it is a negative value. */ \
												current_pc_address = current_pc_address - flipped_relative_address;	/* Flip bits and subtract another one (necessary due to how signed char works). */ \
											else 								/* // If it is a positive value.*/ \
												current_pc_address = current_pc_address + relative_address_low; }

emulation_error object_emulator::emulate(snes_address address, unsigned char databank)
{
	//qWarning() << "Address:" << QString("%1").arg((unsigned int)address, 6, 16, QLatin1Char( '0' ));

	data_bank = databank;
	stack_pointer = 0;

	unsigned int current_pc_address = address.to_pc_format(rom.has_header);

	unsigned char ram_6E;
	unsigned char ram_6F;
	unsigned char ram_screen_number;

	//unsigned char program_bank = 0x00;
	unsigned short program_counter = 0x0000;
	unsigned int current_rom_address = 0x000000;

	unsigned char temp_indirect_bank = 0x00;
	unsigned short temp_indirect_counter = 0x0000;
	unsigned int temp_rom_address = 0x000000;
	unsigned int temp_pc_address;

	direct_page_low = 0x00;
	direct_page_high = 0x00;
	direct_page_bank = 0x7E;

	is_a_8_bit = true;
	is_x_y_8_bit = true;

	// TODO: change JMP parts

	int a;
	for (a=0;a<0x8000;a++)
	{

		program_bank = ROM_ADDRESS_BANK(current_pc_address, rom.has_header);
		ram_6E = ram_buffer.get_byte(0x6E);
		ram_6F = ram_buffer.get_byte(0x6F);
		ram_screen_number = ram_buffer.get_byte(0x1BA1);

		current_rom_address = (ROM_ADDRESS_LOW(current_pc_address, rom.has_header)) | (ROM_ADDRESS_HIGH(current_pc_address, rom.has_header) << 8) | (ROM_ADDRESS_BANK(current_pc_address, rom.has_header) << 16);


		unsigned short negative_bit;
		unsigned short negative_bit_x_y;

		if(is_a_8_bit == true)
			negative_bit = 0x0080;
		else
			negative_bit = 0x8000;

		if(is_x_y_8_bit == true)
			negative_bit_x_y = 0x0080;
		else
			negative_bit_x_y = 0x8000;

		unsigned short bit_mask;
		unsigned short bit_mask_x_y;

		if(is_a_8_bit == true)
			bit_mask = 0x00FF;
		else
			bit_mask = 0xFFFF;

		if(is_x_y_8_bit == true)
			bit_mask_x_y = 0x00FF;
		else
			bit_mask_x_y = 0xFFFF;

		// Addresses and data variables (they are NOT unused variables):
		unsigned int dp_address;
		unsigned short dp_data;

		unsigned char opcode = (rom.get_byte_pc(current_pc_address));



		switch ( opcode ) {
		case 0x9C: OPCODE_MODE_NORMAL(absolute);
		case 0xBC: OPCODE_MODE_NORMAL(indexed_x);
		case 0xBE: OPCODE_MODE_NORMAL(indexed_y);
		default:
		{
			switch ( opcode & 0x1F ) {
			case 0x00: OPCODE_MODE_CONST;
			case 0x01: OPCODE_MODE_DIRECT_PAGE(indexed_indirect);
			case 0x02: OPCODE_MODE_CONST;
			case 0x03: break;
			case 0x04: OPCODE_MODE_DIRECT_PAGE(absolute);
			case 0x05: OPCODE_MODE_DIRECT_PAGE(absolute);
			case 0x06: OPCODE_MODE_DIRECT_PAGE(absolute);
			case 0x07: OPCODE_MODE_DIRECT_PAGE(indirect_long);
			case 0x08: break;
			case 0x09: OPCODE_MODE_CONST;
			case 0x0A: break;
			case 0x0B: break;
			case 0x0C: OPCODE_MODE_NORMAL(absolute);
			case 0x0D: OPCODE_MODE_NORMAL(absolute);
			case 0x0E: OPCODE_MODE_NORMAL(absolute);
			case 0x0F: OPCODE_MODE_LONG(absolute);
			case 0x10: break;
			case 0x11: OPCODE_MODE_DIRECT_PAGE(indirect_indexed);
			case 0x12: OPCODE_MODE_DIRECT_PAGE(indirect);
			case 0x13: break;
			case 0x14: OPCODE_MODE_DIRECT_PAGE(indexed_x);
			case 0x15: OPCODE_MODE_DIRECT_PAGE(indexed_x);
			case 0x16: OPCODE_MODE_DIRECT_PAGE(indexed_x);
			case 0x17: OPCODE_MODE_DIRECT_PAGE(indirect_indexed_long);
			case 0x18: break;
			case 0x19: OPCODE_MODE_NORMAL(indexed_y);
			case 0x1A: break;
			case 0x1B: break;
			case 0x1C: break;
			case 0x1D: OPCODE_MODE_NORMAL(indexed_x);
			case 0x1E: OPCODE_MODE_NORMAL(indexed_x);
			case 0x1F: OPCODE_MODE_LONG(indexed_x);
			}
		}
		}


		//if(current_object == 0x0C)
		//{
		//qWarning() << "Code:" << QString("%1").arg(rom.get_byte_pc(current_pc_address), 2, 16, QLatin1Char( '0' )) << "| Address:" << QString("%1").arg(current_rom_address, 6, 16, QLatin1Char( '0' )) << "| X =" << QString("%1").arg(x_register & bit_mask_x_y, 4 - is_x_y_8_bit * 2, 16, QLatin1Char( '0' )) << "| A =" << QString("%1").arg(a_register & bit_mask, 4 - is_a_8_bit * 2, 16, QLatin1Char( '0' ))
		//			  << "| Y =" << QString("%1").arg(y_register & bit_mask_x_y, 4 - is_x_y_8_bit * 2, 16, QLatin1Char( '0' ))
		//				 << "| $00 =" << QString("%1").arg(ram_buffer.get_byte(0x00), 2, 16, QLatin1Char( '0' ))
		//					<< "| $57 =" << QString("%1").arg(ram_buffer.get_byte(0x57), 2, 16, QLatin1Char( '0' ))
		//					   << "| $02 =" << QString("%1").arg(ram_buffer.get_byte(0x02), 2, 16, QLatin1Char( '0' ))
		//					   << "| $6E =" << QString("%1").arg(ram_buffer.get_byte(0x6e), 2, 16, QLatin1Char( '0' ))
		//					   << "| $6F =" << QString("%1").arg(ram_buffer.get_byte(0x6f), 2, 16, QLatin1Char( '0' ))
		//					   << "| $70 =" << QString("%1").arg(ram_buffer.get_byte(0x70), 2, 16, QLatin1Char( '0' ))
		//					   << "| carry =" << QString("%1").arg(carry_flag, 2, 16, QLatin1Char( '0' ));
		//}
		//qWarning() << "Data Bank:" << QString("%1").arg(data_bank, 2, 16, QLatin1Char( '0' ));

		switch ( rom.get_byte_pc(current_pc_address) ) {
		case 0x00:	return emulation_error::unused_opcode;				// BRK (unsupported)
		case 0x01:	OPERATOR_ORA;	NEXT(2);	break;	// ORA (_dp,_X)
		case 0x02:	return emulation_error::unused_opcode;				// COP (enables coprocessor, unsupported)
		case 0x03:	return emulation_error::unused_opcode;				// ORA sr,S (unsupported, stack relative)
		case 0x04:	return emulation_error::unused_opcode;				// TSB (unsupported for now)
		case 0x05:	OPERATOR_ORA;	NEXT(2);	break;	// ORA dp
		case 0x06:	OPERATOR_ASL_ADDR;	NEXT(2);	break;	// ASL dp
		case 0x07:	OPERATOR_ORA;	NEXT(2);	break;	// ORA [_dp_]
		case 0x08:	return emulation_error::unused_opcode;
		case 0x09:	OPERATOR_ORA;	NEXT_CONST;	break;	// ORA #const
		case 0x0A:	OPERATOR_ASL_A;	NEXT(1);	break;	// ASL A
		case 0x0B:	return emulation_error::unused_opcode;				// TCS Transfer to Stack Pointer (unsupported, not recommended)
		case 0x0C:	return emulation_error::unused_opcode;				// TSB (unsupported for now)
		case 0x0D:	OPERATOR_ORA;	NEXT(3);	break;	// ORA addr
		case 0x0E:	OPERATOR_ASL_ADDR;	NEXT(3);	break;	// ASL addr
		case 0x0F:	OPERATOR_ORA;	NEXT(4);	break;	// ORA long
		case 0x10:	if(negative_flag == false)	OPERATOR_BRA;	NEXT(2);	break;// BPL (Branch if plus)
		case 0x11:	OPERATOR_ORA;	NEXT(2);	break;	// ORA (_dp_),Y
		case 0x12:	OPERATOR_ORA;	NEXT(2);	break;	// ORA (_dp_)
		case 0x13:	return emulation_error::unused_opcode;				// ORA (_sr_,S),Y (unsupported, stack relative)
		case 0x14:	return emulation_error::unused_opcode;				// TRB (unsupported for now)
		case 0x15:	OPERATOR_ORA;	NEXT(2);	break;	// ORA dp,X
		case 0x16:	OPERATOR_ASL_ADDR;	NEXT(2);	break;	// ASL dp,X
		case 0x17:	OPERATOR_ORA;	NEXT(2);	break;	// ORA [_dp_],Y
		case 0x18:	carry_flag = false;	NEXT(1);	break;	// CLC (Clear Carry)
		case 0x19:	OPERATOR_ORA;	NEXT(3);	break;	// ORA addr,Y
		case 0x1A:	OPERATOR_INC_A;	NEXT(1);	break;	// INC A
		case 0x1B:	return emulation_error::unused_opcode;
		case 0x1C:	return emulation_error::unused_opcode;	// TRB (unsupported for now)
		case 0x1D:	OPERATOR_ORA;	NEXT(3);	break;	// ORA addr,X
		case 0x1E:	OPERATOR_ASL_ADDR;	NEXT(3);	break;	// ASL addr,X
		case 0x1F:	OPERATOR_ORA;	NEXT(4);	break;	// ORA long,X
		case 0x20:	// JSR addr
		{
			current_pc_address += 2; // One less because of weird snes behavior (it adds one to the address at RTS)


			push_stack(ROM_ADDRESS_HIGH(current_pc_address, rom.has_header));
			push_stack(ROM_ADDRESS_LOW(current_pc_address, rom.has_header));

			unsigned char address_low = rom.get_byte_pc(current_pc_address - 1);
			unsigned char address_high = rom.get_byte_pc(current_pc_address - 0);
			unsigned int address = get_address(address_low, address_high, program_bank, absolute);

			current_pc_address = PC_ADDRESS(address, rom.has_header);

			break;
		}
		case 0x21:	OPERATOR_AND;	NEXT(2);	break;	// AND (_dp_,X)
		case 0x22:	// JSL long
		{
			current_pc_address += 3;	// One less due to weird snes behavior (it adds one to the address when RTL)

			push_stack(ROM_ADDRESS_BANK(current_pc_address, rom.has_header));
			push_stack(ROM_ADDRESS_HIGH(current_pc_address, rom.has_header));
			push_stack(ROM_ADDRESS_LOW(current_pc_address, rom.has_header));


			unsigned char address_low = rom.get_byte_pc(current_pc_address - 2);
			unsigned char address_high = rom.get_byte_pc(current_pc_address - 1);
			unsigned char address_bank = rom.get_byte_pc(current_pc_address - 0);
			unsigned int address = get_address(address_low, address_high, address_bank, absolute);

			current_pc_address = PC_ADDRESS(address, rom.has_header);


			break;
		}
		case 0x23:	return emulation_error::unused_opcode;	// (unsupported, stack relative)
		case 0x24:	OPERATOR_BIT;	NEXT(2);	break;	// BIT dp
		case 0x25:	OPERATOR_AND;	NEXT(2);	break;	// AND dp
		case 0x26:	OPERATOR_ROL;	NEXT(2);	break;	// ROL dp
		case 0x27:	OPERATOR_AND;	NEXT(2);	break;	// AND [_dp_]
		case 0x28:	return emulation_error::unused_opcode;
		case 0x29:	OPERATOR_AND;	NEXT_CONST;	break;	// AND #const
		case 0x2A:	OPERATOR_ROL_A;	NEXT(1);	break;	// ROL A
		case 0x2B:	return emulation_error::unused_opcode;
		case 0x2C:	OPERATOR_BIT;		NEXT(3);	break;	// BIT addr
		case 0x2D:	OPERATOR_AND;		NEXT(3);	break;	// AND addr
		case 0x2E:	OPERATOR_ROL;		NEXT(3);	break;	// ROL addr
		case 0x2F:	OPERATOR_AND;		NEXT(4);	break;	// AND long
		case 0x30:	if(negative_flag == true)	OPERATOR_BRA;	NEXT(2);	break;// BMI (Branch if minus)
		case 0x31:	OPERATOR_AND;		NEXT(2);	break;	// AND (_dp_),y
		case 0x32:	OPERATOR_AND;		NEXT(2);	break;	// AND (_dp_)
		case 0x33:	return emulation_error::unused_opcode;	// (unsupported, stack relative)
		case 0x34:	OPERATOR_BIT;		NEXT(2);	break;	// BIT dp,X
		case 0x35:	OPERATOR_AND;		NEXT(2);	break;	// AND dp,X
		case 0x36:	OPERATOR_ROL;		NEXT(2);	break;	// ROL dp,X
		case 0x37:	OPERATOR_AND;		NEXT(2);	break;	// AND [_dp_],Y
		case 0x38:	carry_flag = true;	NEXT(1);	break;	// SEC
		case 0x39:	OPERATOR_AND;		NEXT(3);	break;	// AND addr,Y
		case 0x3A:	OPERATOR_DEC_A;		NEXT(1);	break;	// DEC A
		case 0x3B:	return emulation_error::unused_opcode;
		case 0x3C:	OPERATOR_BIT;		NEXT(3);	break;	// BIT addr,X
		case 0x3D:	OPERATOR_AND;		NEXT(3);	break;	// AND addr,X
		case 0x3E:	OPERATOR_ROL;		NEXT(3);	break;	// ROL addr,X
		case 0x3F:	OPERATOR_AND;		NEXT(4);	break;	// AND long,X
		case 0x40:	return emulation_error::unused_opcode;
		case 0x41:	OPERATOR_EOR;		NEXT(2);	break;	// EOR (_dp_,X)
		case 0x42:	return emulation_error::unused_opcode;
		case 0x43:	return emulation_error::unused_opcode;	// (unsupported, stack relative)
		case 0x44:	// MVP
		{
			unsigned char source_bank = rom.get_byte_pc(current_pc_address + 1);
			data_bank = rom.get_byte_pc(current_pc_address + 2);

			for(int temp_a = (a_register & bit_mask); temp_a>-1; temp_a--)
			{
				unsigned char temp_data = get_ram_rom_byte(((source_bank << 16) | (x_register & bit_mask_x_y)));
				set_ram_rom_byte((data_bank << 16) | (y_register & bit_mask_x_y), temp_data);
				y_register--;
				x_register--;
			}
			a_register = (a_register & (0xFF00 * is_a_8_bit)) | bit_mask;

			NEXT(3); break;
		}
		case 0x45:	OPERATOR_EOR;		NEXT(2);	break;	// EOR dp
		case 0x46:	OPERATOR_LSR_ADDR;	NEXT(2);	break;	// LSR dp
		case 0x47:	OPERATOR_EOR;		NEXT(2);	break;	// EOR [_dp_]
		case 0x48:
		{
			if(is_a_8_bit == false)
				push_stack((a_register >> 8) & 0xFF);

			push_stack(a_register & 0xFF);

			current_pc_address++;
			break;
		}
		case 0x49:	OPERATOR_EOR;		NEXT_CONST;	break;	// EOR #const
		case 0x4A:	// LSR A
		{
			carry_flag = a_register & 0x01;

			// Perform RIGHTSHIFT:
			a_register = (a_register & (0xFF00 * is_a_8_bit)) | (((a_register & bit_mask) >> 1) & bit_mask);

			// Set flags:
			zero_flag = !(a_register & bit_mask);
			negative_flag = a_register & negative_bit;

			current_pc_address++;

			break;
		}
		case 0x4B:	// PHK (push program bank)
		{
			push_stack(program_bank & 0xFF);

			current_pc_address++;
			break;
		}
		case 0x4C:	// JMP addr {absolute}
		{
			unsigned char address_low = rom.get_byte_pc(current_pc_address + 1);
			unsigned char address_high = rom.get_byte_pc(current_pc_address + 2);
			unsigned int address = get_address(address_low, address_high, program_bank, absolute);

			current_pc_address = PC_ADDRESS(address, rom.has_header);

			break;
		}
		case 0x4D:	OPERATOR_EOR;		NEXT(3);	break;	// EOR addr
		case 0x4E:	OPERATOR_LSR_ADDR;	NEXT(3);	break;	// LSR addr
		case 0x4F:	OPERATOR_EOR;		NEXT(4);	break;	// EOR long
		case 0x50:	if(overflow_flag == false)	OPERATOR_BRA;	NEXT(2);	break;	// BVC (Branch if overflow clear)
		case 0x51:	OPERATOR_EOR;		NEXT(2);	break;	// EOR (_dp_),Y
		case 0x52:	OPERATOR_EOR;		NEXT(2);	break;	// EOR (_dp_)
		case 0x53:	return emulation_error::unused_opcode; // (unsupported, stack relative)
		case 0x54:	 // MVN (move block negative)
		{
			unsigned char source_bank = rom.get_byte_pc(current_pc_address + 1);
			data_bank = rom.get_byte_pc(current_pc_address + 2);

			for(int temp_a = (a_register & bit_mask); temp_a>-1; temp_a--)
			{
				unsigned char temp_data = get_ram_rom_byte(((source_bank << 16) | (x_register & bit_mask_x_y)));
				set_ram_rom_byte((data_bank << 16) | (y_register & bit_mask_x_y), temp_data);
				y_register++;
				x_register++;
			}
			a_register = (a_register & (0xFF00 * is_a_8_bit)) | bit_mask;

			NEXT(3); break;
		}
		case 0x55:	OPERATOR_EOR;		NEXT(2);	break;	// EOR dp,X
		case 0x56:	OPERATOR_LSR_ADDR;	NEXT(2);	break;	// LSR dp,X
		case 0x57:	OPERATOR_EOR;		NEXT(2);	break;	// EOR [_dp_],Y
		case 0x58:	return emulation_error::unused_opcode;				// Interrups can't be disabled.
		case 0x59:	OPERATOR_EOR;		NEXT(3);	break;	// EOR addr,Y
		case 0x5A:	// Push Y
		{
			if(is_x_y_8_bit == false)
				push_stack((y_register >> 8) & 0xFF);

			push_stack(y_register & 0xFF);

			current_pc_address++;
			break;
		}
		case 0x5B:	// TCD Transfer to Direct Page Register (unsupported, not recommended)
		{
			direct_page_low = a_register & 0xFF;
			direct_page_high = (a_register & 0xFF00) >> 8;

			NEXT(1);
			break;
		}
		case 0x5C:	// JML long {absolute long}
		{
			unsigned char address_low = rom.get_byte_pc(current_pc_address + 1);
			unsigned char address_high = rom.get_byte_pc(current_pc_address + 2);
			unsigned char address_bank = rom.get_byte_pc(current_pc_address + 3);
			unsigned int address = get_address(address_low, address_high, address_bank, absolute);

			current_pc_address = PC_ADDRESS(address, rom.has_header);

			break;
		}
		case 0x5D:	OPERATOR_EOR;		NEXT(3);	break;	// EOR addr,X
		case 0x5E:	OPERATOR_LSR_ADDR;	NEXT(3);	break;	// LSR addr,X
		case 0x5F:	OPERATOR_EOR;		NEXT(4);	break;	// EOR long,X
		case 0x60:	// RTS (Return from subroutine)
		{
			// If stack is empty, finish emulation.
			if(stack_pointer == 0)
				return emulation_error::emulator_success;

			// Reverse order, very important:
			unsigned char address_low = pull_stack();
			unsigned char address_high = pull_stack();


			unsigned int address = get_address(address_low, address_high, program_bank, absolute) + 1;

			current_pc_address = PC_ADDRESS(address, rom.has_header);

			break;
		}
		case 0x61:	OPERATOR_ADC;		NEXT(2);	break;	// ADC (_dp_,X)
		case 0x62:	return emulation_error::unused_opcode;
		case 0x63:	return emulation_error::unused_opcode;	// ADC sr,S (unsupported, stack relative)
		case 0x64:	OPERATOR_STZ;		NEXT(2);	break;	// STZ dp
		case 0x65:	OPERATOR_ADC;		NEXT(2);	break;	// ADC dp
		case 0x66:	OPERATOR_ROR_ADDR;	NEXT(2);	break;	// ROR dp
		case 0x67:	OPERATOR_ADC;		NEXT(2);	break;	// ADC [_dp_]
		case 0x68:	// Pull a
		{

			// LOW byte is pulled first.
			a_register = pull_stack();

			if(is_a_8_bit == false)
				a_register = (a_register & 0x00FF) | (pull_stack() << 8);

			current_pc_address++;
			break;
		}
		case 0x69:	OPERATOR_ADC;		NEXT_CONST;	break;	// ADC #const
		case 0x6A:	// ROR A
		{
			carry_flag = a_register & 0x01;

			// Perform RIGHTROLL:
			a_register = ror(a_register, 1, is_a_8_bit);

			// Set flags:
			zero_flag = !(a_register & bit_mask);
			negative_flag = a_register & negative_bit;

			current_pc_address++;

			break;
		}
		case 0x6B:	// RTL
		{
			// If stack is empty, finish emulation.
			if(stack_pointer == 0)
				return emulation_error::emulator_success;

			// Reverse order, very important:

			unsigned char address_low = pull_stack();
			unsigned char address_high = pull_stack();
			unsigned char address_bank = pull_stack();

			unsigned int address = get_address(address_low, address_high, address_bank, absolute) + 1;

			current_pc_address = PC_ADDRESS(address, rom.has_header);

			break;
		}
		case 0x6C:	// JMP (_addr_) {absolute indirect)
		{
			unsigned char address_low = rom.get_byte_pc(current_pc_address + 1);
			unsigned char address_high = rom.get_byte_pc(current_pc_address + 2);
			unsigned int address = get_address(address_low, address_high, data_bank, indirect);				// TODO: is this DATA_BANK or PROGRAM_BANK?

			current_pc_address = PC_ADDRESS(address, rom.has_header);
			break;
		}
		case 0x6D:	OPERATOR_ADC;		NEXT(3);	break;	// ADC addr
		case 0x6E:	OPERATOR_ROR_ADDR;	NEXT(3);	break;	// ROR addr
		case 0x6F:	OPERATOR_ADC;		NEXT(4);	break;	// ADC long
		case 0x70:	if(overflow_flag == true)	OPERATOR_BRA;	NEXT(2);	break;// BVS (Branch if overflow set)
		case 0x71:	OPERATOR_ADC;		NEXT(2);	break;	// ADC (dp),Y
		case 0x72:	OPERATOR_ADC;		NEXT(2);	break;	// ADC (_dp_)
		case 0x73:	return emulation_error::unused_opcode;	// ADC (_sr_,S),Y (unsupported, stack relative)
		case 0x74:	OPERATOR_STZ;		NEXT(2);	break;	// STZ dp,X
		case 0x75:	OPERATOR_ADC;		NEXT(2);	break;	// ADC dp,X
		case 0x76:	OPERATOR_ROR_ADDR;	NEXT(2);	break;	// ROR dp,X
		case 0x77:	OPERATOR_ADC;		NEXT(2);	break;	// ADC [_dp_],Y
		case 0x78:	return emulation_error::unused_opcode;
		case 0x79:	OPERATOR_ADC;		NEXT(3);	break;	// ADC addr,Y
		case 0x7A:	// Pull Y
		{

			// LOW byte is pulled first.
			y_register = pull_stack();

			if(is_x_y_8_bit == false)
				y_register = (y_register & 0x00FF) | (pull_stack() << 8);

			current_pc_address++;
			break;
		}
		case 0x7B:	// TDC Transfer from Direct Page register (unsupported, not recommended)
		{
			a_register = direct_page_low & (direct_page_high << 8);

			NEXT(1);
			break;
		}
		case 0x7C:	// JMP (_addr_,X) {absolute indexed indirect}
			temp_indirect_counter = rom.get_byte_pc(current_pc_address + 1) | (rom.get_byte_pc(current_pc_address + 2) << 8);
			temp_rom_address = (temp_indirect_counter | (program_bank << 16)) + x_register;
			temp_pc_address = PC_ADDRESS(temp_rom_address, rom.has_header);

			program_counter = rom.get_byte_pc(temp_pc_address + 1) | (rom.get_byte_pc(temp_pc_address + 2) << 8);
			current_rom_address = program_counter | (program_bank << 16);
			current_pc_address = PC_ADDRESS(current_rom_address, rom.has_header);
			break;
		case 0x7D:	OPERATOR_ADC;		NEXT(3);	break;	// ADC addr,X
		case 0x7E:	OPERATOR_ROR_ADDR;	NEXT(3);	break;	// ROR addr,X
		case 0x7F:	OPERATOR_ADC;		NEXT(4);	break;	// ADC long,X
		case 0x80:	OPERATOR_BRA;		NEXT(2);	break;	// BRA always
		case 0x81:	OPERATOR_STA;		NEXT(2);	break;	// STA (_dp_,X)
		case 0x82:	// BRL always
		{
			unsigned char relative_address_low = rom.get_byte_pc(current_pc_address + 1);
			unsigned char relative_address_high = rom.get_byte_pc(current_pc_address + 2);
			unsigned short relative_address = (relative_address_low) | (relative_address_high << 8);
			unsigned short flipped_relative_address = (~relative_address) + 1;

			if((relative_address & 0x8000) == 0x8000)	// If it is a negative value.
				current_pc_address = current_pc_address - flipped_relative_address;	// Flip bits and subtract another one (necessary due to how signed char works).
			else 								// If it is a positive value.
				current_pc_address = current_pc_address + relative_address;

			current_pc_address += 3;
			break;
		}
		case 0x83:	return emulation_error::unused_opcode; // (unsupported, stack relative)
		case 0x84:	OPERATOR_STY;		NEXT(2);	break;	// STY dp
		case 0x85:	OPERATOR_STA;		NEXT(2);	break;	// STA dp
		case 0x86:	OPERATOR_STX;		NEXT(2);	break;	// STX dp
		case 0x87:	OPERATOR_STA;		NEXT(2);	break;	// STA [_dp_]
		case 0x88:	// DEY
			y_register--;

			// Set flags:
			zero_flag = !(y_register & bit_mask_x_y);
			negative_flag = y_register & negative_bit_x_y;

			current_pc_address++;
			break;
		case 0x89:	OPERATOR_BIT;		NEXT_CONST;	break;	// BIT #const
		case 0x8A:	// TXA (Transfer X to A)
		{
			a_register = x_register;

			// Set flags:
			zero_flag = !(a_register & bit_mask);
			negative_flag = a_register & negative_bit;

			current_pc_address++;

			break;
		}
		case 0x8B:	// PHB
		{
			push_stack(data_bank & 0xFF);

			current_pc_address++;
			break;
		}
		case 0x8C:	OPERATOR_STY;		NEXT(3);	break;	// STY addr
		case 0x8D:	OPERATOR_STA;		NEXT(3);	break;	// STA addr
		case 0x8E:	OPERATOR_STX;		NEXT(3);	break;	// STX addr
		case 0x8F:	OPERATOR_STA;		NEXT(4);	break;	// STA long
		case 0x90:	if(carry_flag == false)	OPERATOR_BRA;	NEXT(2);	break;	// BCC nearlabel
		case 0x91:	OPERATOR_STA;		NEXT(2);	break;	// STA (_dp_),Y
		case 0x92:	OPERATOR_STA;		NEXT(2);	break;	// STA (_dp_)
		case 0x93:	return emulation_error::unused_opcode; // (unsupported, stack relative)
		case 0x94:	OPERATOR_STY;		NEXT(2);	break;	// STY dp,X
		case 0x95:	OPERATOR_STA;		NEXT(2);	break;	// STA dp,X
		case 0x96:	OPERATOR_STX;		NEXT(2);	break;	// STX dp,Y
		case 0x97:	OPERATOR_STA;		NEXT(2);	break;	// STA [_dp_],Y
		case 0x98:	// TYA (Transfer Y to A)
		{
			a_register = y_register;

			// Set flags:
			zero_flag = !(a_register & bit_mask);
			negative_flag = a_register & negative_bit;

			current_pc_address++;

			break;
		}
		case 0x99:	OPERATOR_STA;		NEXT(3);	break;	// STA addr,Y
		case 0x9A:	return emulation_error::unused_opcode;
		case 0x9B:	// TXY (Transfer X to Y)
		{
			y_register = x_register;

			// Set flags:
			zero_flag = !(y_register & bit_mask_x_y);
			negative_flag = y_register & negative_bit_x_y;

			current_pc_address++;

			break;
		}
		case 0x9C:	OPERATOR_STZ;		NEXT(3);		break;	// STZ addr
		case 0x9D:	OPERATOR_STA;		NEXT(3);		break;	// STA addr,X
		case 0x9E:	OPERATOR_STZ;		NEXT(3);		break;	// STZ addr,X
		case 0x9F:	OPERATOR_STA;		NEXT(4);		break;	// STA long,X
		case 0xA0:	OPERATOR_LDY;		NEXT_CONST_XY;	break;	// LDY #const
		case 0xA1:	OPERATOR_LDA;		NEXT(2);		break;	// LDA (_dp_,X)
		case 0xA2:	OPERATOR_LDX;		NEXT_CONST_XY	break;	// LDX #const
		case 0xA3:	return emulation_error::unused_opcode; // (unsupported, stack relative)
		case 0xA4:	OPERATOR_LDY;		NEXT(2);	break;	// LDY dp
		case 0xA5:	OPERATOR_LDA;		NEXT(2);	break;	// LDA dp
		case 0xA6:	OPERATOR_LDX;		NEXT(2);	break;	// LDX dp
		case 0xA7:	OPERATOR_LDA;		NEXT(2);	break;	// LDA [_dp_]
		case 0xA8:	// TAY (Tranfer A to Y)
		{
			y_register = a_register;

			// Set flags:
			zero_flag = !(y_register & bit_mask_x_y);
			negative_flag = y_register & negative_bit_x_y;

			current_pc_address++;

			break;
		}
		case 0xA9:	OPERATOR_LDA;		NEXT_CONST;	break;	// LDA #const
		case 0xAA:	// TAX	(Transfer A to X)
		{
			x_register = a_register;

			// Set flags:
			zero_flag = !(x_register & bit_mask_x_y);
			negative_flag = x_register & negative_bit_x_y;

			current_pc_address++;

			break;
		}
		case 0xAB:	// PLB
		{
			data_bank = pull_stack();

			current_pc_address++;
			break;
		}
		case 0xAC:							OPERATOR_LDY;			NEXT(3);	break;	// LDY addr
		case 0xAD:							OPERATOR_LDA;			NEXT(3);	break;	// LDA addr
		case 0xAE:							OPERATOR_LDX;			NEXT(3);	break;	// LDX addr
		case 0xAF:							OPERATOR_LDA;			NEXT(4);	break;	// LDA long
		case 0xB0:	if(carry_flag == true)	OPERATOR_BRA;			NEXT(2);	break;	// BCS nearlabel
		case 0xB1:							OPERATOR_LDA;			NEXT(2);	break;	// LDA (_dp_),Y
		case 0xB2:							OPERATOR_LDA;			NEXT(2);	break;	// LDA (_dp_)
		case 0xB3:	return emulation_error::unused_opcode;												// (unsupported, stack relative)
		case 0xB4:							OPERATOR_LDY;			NEXT(2);	break;	// LDY dp,X
		case 0xB5:							OPERATOR_LDA;			NEXT(2);	break;	// LDA dp,X
		case 0xB6:							OPERATOR_LDX;			NEXT(2);	break;	// LDX dp,Y
		case 0xB7:							OPERATOR_LDA;			NEXT(2);	break;	// LDA [_dp_],Y
		case 0xB8:							overflow_flag = false;	NEXT(1);	break;
		case 0xB9:							OPERATOR_LDA;			NEXT(3);	break;	// LDA addr,Y
		case 0xBA:	return emulation_error::unused_opcode;
		case 0xBB:	// TYX (Transfer Y to X)
		{
			x_register = y_register;

			// Set flags:
			zero_flag = !(x_register & bit_mask_x_y);
			negative_flag = x_register & negative_bit_x_y;

			current_pc_address++;

			break;
		}
		case 0xBC:	OPERATOR_LDY;		NEXT(3);	break;	// LDY addr,X
		case 0xBD:	OPERATOR_LDA;		NEXT(3);	break;	// LDA addr,X
		case 0xBE:	OPERATOR_LDX;		NEXT(3);	break;	// LDX addr,Y
		case 0xBF:	OPERATOR_LDA;		NEXT(4);	break;	// LDA long,X
		case 0xC0:	OPERATOR_CPY;		NEXT_CONST_XY;	break;	// CPY #const
		case 0xC1:	OPERATOR_CMP;		NEXT(2);	break;	// CMP (_dp_,X)
		case 0xC2:	// REP #const
		{
			unsigned char data = rom.get_byte_pc(current_pc_address + 1);

			if(data & 0x01)
				carry_flag = false;
			if(data & 0x02)
				zero_flag = false;
			//if(data & 0x04)
				//irq_disable = false;
				// TODO: add warning messages that these modes should not be used!
			//if(data & 0x08)
				//decimal_flag = false;
			if(data & 0x10)
				is_x_y_8_bit = false;
			if(data & 0x20)
				is_a_8_bit = false;
			if(data & 0x40)
				overflow_flag = false;
			if(data & 0x80)
				negative_flag = false;

			current_pc_address += 2;

			break;
		}
		case 0xC3:	return emulation_error::unused_opcode; // (unsupported, stack relative)
		case 0xC4:	OPERATOR_CPY;		NEXT(2);	break;	// CPY dp
		case 0xC5:	OPERATOR_CMP;		NEXT(2);	break;	// CMP dp
		case 0xC6:	OPERATOR_DEC_ADDR;	NEXT(2);	break;	// DEC dp
		case 0xC7:	OPERATOR_CMP;		NEXT(2);	break;	// CMP [_dp_]
		case 0xC8:	// INY
			y_register++;

			// Set flags:
			zero_flag = !(y_register & bit_mask_x_y);
			negative_flag = y_register & negative_bit_x_y;

			current_pc_address++;
			break;
		case 0xC9:	OPERATOR_CMP;		NEXT_CONST;	break;	// CMP #const
		case 0xCA: // DEX
		{
			x_register--;

			// Set flags:
			zero_flag = !(x_register & bit_mask_x_y);
			negative_flag = x_register & negative_bit_x_y;

			current_pc_address++;
			break;
		}
		case 0xCB:	return emulation_error::unused_opcode;
		case 0xCC:	OPERATOR_CPY;		NEXT(3);	break;	// CPY addr
		case 0xCD:	OPERATOR_CMP;		NEXT(3);	break;	// CMP addr
		case 0xCE:	OPERATOR_DEC_ADDR;	NEXT(3);	break;	// DEC addr
		case 0xCF:	OPERATOR_CMP;		NEXT(4);	break;	// CMP long
		case 0xD0:	if(zero_flag == false)	OPERATOR_BRA;	NEXT(2);	break;	// BNE (Branch if not zero)
		case 0xD1:	OPERATOR_CMP;		NEXT(2);	break;	// CMP (_dp_),Y
		case 0xD2:	OPERATOR_CMP;		NEXT(2);	break;	// CMP (_dp_)
		case 0xD3:	return emulation_error::unused_opcode; // (unsupported, stack relative)
		case 0xD4:	return emulation_error::unused_opcode;
		case 0xD5:	OPERATOR_CMP;		NEXT(2);	break;	// CMP dp,X
		case 0xD6:	OPERATOR_DEC_ADDR;	NEXT(2);	break;	// DEC dp,X
		case 0xD7:	OPERATOR_CMP;		NEXT(2);	break;	// CMP [_dp_],Y
		case 0xD8:	return emulation_error::unused_opcode;	// SLD (clear decimal mode flag, unsupported)
		case 0xD9:	OPERATOR_CMP;		NEXT(3);	break;	// CMP addr,Y
		case 0xDA:	// Push X
		{
			if(is_x_y_8_bit == false)
				push_stack((x_register >> 8) & 0xFF);

			push_stack(x_register & 0xFF);

			current_pc_address++;
			break;
		}
		case 0xDB:	return emulation_error::unused_opcode; // STP (stop processor, unsupported)
		case 0xDC:	// JMP [addr] {absolute indirect long}
		{

			unsigned char address_low = rom.get_byte_pc(current_pc_address + 1);
			unsigned char address_high = rom.get_byte_pc(current_pc_address + 2);
			unsigned int address = get_address(address_low, address_high, data_bank, indirect_long);


			current_pc_address = PC_ADDRESS(address, rom.has_header);

			break;
		}
		case 0xDD:	OPERATOR_CMP;		NEXT(3);	break;	// CMP addr,X
		case 0xDE:	OPERATOR_DEC_ADDR;	NEXT(3);	break;	// DEC addr,X
		case 0xDF:	OPERATOR_CMP;		NEXT(4);	break;	// CMP long,X
		case 0xE0:	OPERATOR_CPX;		NEXT_CONST_XY;		break;	// CPX #const
		case 0xE1:	OPERATOR_SBC;		NEXT(2);	break;	// SBC (_dp_,X)
		case 0xE2:	// SEP #const
		{
			unsigned char data = rom.get_byte_pc(current_pc_address + 1);

			if(data & 0x01)
				carry_flag = true;
			if(data & 0x02)
				zero_flag = true;
			//if(data & 0x04)
				//irq_disable = false;
				// TODO: add warning messages that these modes should not be used!
			//if(data & 0x08)
				//decimal_flag = false;
			if(data & 0x10)
				is_x_y_8_bit = true;
			if(data & 0x20)
				is_a_8_bit = true;
			if(data & 0x40)
				overflow_flag = true;
			if(data & 0x80)
				negative_flag = true;

			current_pc_address += 2;

			break;
		}
		case 0xE3:	return emulation_error::unused_opcode; // (unsupported, stack relative)
		case 0xE4:	OPERATOR_CPX;		NEXT(2);		break;	// CPX dp
		case 0xE5:	OPERATOR_SBC;		NEXT(2);	break;	// SBC dp
		case 0xE6:	OPERATOR_INC_ADDR;	NEXT(2);	break;	// INC dp
		case 0xE7:	OPERATOR_SBC;		NEXT(2);	break;	// SBC [_dp_]
		case 0xE8:	// INX
			x_register++;

			// Set flags:
			zero_flag = !(x_register & bit_mask_x_y);
			negative_flag = x_register & negative_bit_x_y;

			current_pc_address++;
			break;
		case 0xE9:	OPERATOR_SBC;		NEXT_CONST;	break;	// SBC #const
		case 0xEA:						NEXT(1);	break;	// NOP No operation, skip to next.
		case 0xEB:	// XBA (transfer a_high to a_low and a_low to a_high)
		{
			unsigned char a_byte = a_register & 0xFF;
			unsigned char b_byte = (a_register >> 8) & 0xFF;

			// Exchange them:
			a_register = b_byte | (a_byte << 8);

			zero_flag = !(a_register & bit_mask);
			negative_flag = a_register & negative_bit;

			current_pc_address++;

			break;
		}
		case 0xEC:	OPERATOR_CPX;		NEXT(3);	break;	// CPX addr
		case 0xED:	OPERATOR_SBC;		NEXT(3);	break;	// SBC addr
		case 0xEE:	OPERATOR_INC_ADDR;	NEXT(3);	break;	// INC addr
		case 0xEF:	OPERATOR_SBC;		NEXT(4);	break;	// SBC long
		case 0xF0:	if(zero_flag == true)	OPERATOR_BRA;	NEXT(2);	break;	// BEQ (branch if zero)
		case 0xF1:	OPERATOR_SBC;		NEXT(2);	break;	// SBC (_dp_),Y
		case 0xF2:	OPERATOR_SBC;		NEXT(2);	break;	// SBC (_dp_)
		case 0xF3:	return emulation_error::unused_opcode;					// (unsupported, stack relative)
		case 0xF4:	return emulation_error::unused_opcode;
		case 0xF5:	OPERATOR_SBC;		NEXT(2);	break;	// SBC dp,X
		case 0xF6:	OPERATOR_INC_ADDR;	NEXT(2);	break;	// INC dp,X
		case 0xF7:	OPERATOR_SBC;		NEXT(2);	break;	// SBC [_dp_],Y
		case 0xF8:	return emulation_error::unused_opcode;
		case 0xF9:	OPERATOR_SBC;		NEXT(3);	break;	// SBC addr,Y
		case 0xFA:	// Pull X
		{

			// LOW byte is pulled first.
			x_register = pull_stack();

			if(is_x_y_8_bit == false)
				x_register = (x_register & 0x00FF) | (pull_stack() << 8);

			current_pc_address++;
			break;
		}
		case 0xFB:	return emulation_error::unused_opcode;	// XCE Exchange Carry and Emulation Flags (unsupported, not recommended)
		case 0xFC:	// JSR (addr,X)
		{
			current_pc_address += 2;	// One less due to weird behavior of the snes when using RTS (it adds one there).


			push_stack(ROM_ADDRESS_HIGH(current_pc_address, rom.has_header));
			push_stack(ROM_ADDRESS_LOW(current_pc_address, rom.has_header));

			unsigned char address_low = rom.get_byte_pc(current_pc_address - 1);
			unsigned char address_high = rom.get_byte_pc(current_pc_address - 0);
			unsigned int address = get_address(address_low, address_high, program_bank, indexed_indirect);

			current_pc_address = PC_ADDRESS(address, rom.has_header);

			break;
		}
		case 0xFD:	OPERATOR_SBC;		NEXT(3);	break;	// SBC addr,X
		case 0xFE:	OPERATOR_INC_ADDR;	NEXT(3);	break;	// INC addr,X
		case 0xFF:	OPERATOR_SBC;		NEXT(4);	break;	// SBC long,X
		default:
			return emulation_error::unused_opcode;
			break;
		}
	}
	qWarning() << "WARNING: Emulator could not finish.";
	return emulation_error::emulator_stall;
}
