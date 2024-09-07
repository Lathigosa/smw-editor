#include "main.h"

bool load_map16_tile(map16_map &output_buffer, unsigned char page, unsigned char index, const SMW_ROM &rom, unsigned int rom_address)
{
	map16_gfx_tile temp_data;

	unsigned int address = PC_ADDRESS(rom_address, rom.has_header);

	temp_data.act_as_hi = page;
	temp_data.act_as_lo = index;

	unsigned int a;
	for (a=0;a<4;a++)
	{
		temp_data.gfx_tiles[a].low_byte  =  rom.get_byte_pc(address + 0);
		temp_data.gfx_tiles[a].high_byte =  rom.get_byte_pc(address + 1) & 0x03;

		temp_data.gfx_tiles[a].priority  =  rom.get_byte_pc(address + 1) & 0x20;
		temp_data.gfx_tiles[a].flip_x    =  rom.get_byte_pc(address + 1) & 0x40;
		temp_data.gfx_tiles[a].flip_y    =  rom.get_byte_pc(address + 1) & 0x80;
		temp_data.gfx_tiles[a].palette   = (rom.get_byte_pc(address + 1) & 0x1C) >> 2;

		address += 2;
	}

	output_buffer.setBlock(page, index, temp_data);

	return true;
}

bool load_vanilla_map16(map16_map &output_buffer, const SMW_ROM &rom, unsigned char tileset)
{
	if(output_buffer.getPageCount() < 2)	// If there isn't enough room for the loaded data, return.
		return false;

	unsigned int rom_address = 0x0D8000;

	unsigned int index;

	// Load tiles 000-072 (all tilesets):
	for (index=0;index<=0x72;index++)
	{
		load_map16_tile(output_buffer, 0, index, rom, rom_address);
		rom_address += 8;
	}

	// Load tiles 100-106 (all tilesets):
	rom_address = 0x0D8398;
	for (index=0;index<=0x06;index++)
	{
		load_map16_tile(output_buffer, 1, index, rom, rom_address);
		rom_address += 8;
	}

	// Load tiles 111-152 (all tilesets):
	rom_address = 0x0D83D0;
	for (index=0x11;index<=0x52;index++)
	{
		load_map16_tile(output_buffer, 1, index, rom, rom_address);
		rom_address += 8;
	}

	// Load tiles 16E-1FF (all tilesets):
	rom_address = 0x0D85E0;
	for (index=0x6E;index<=0xFF;index++)
	{
		load_map16_tile(output_buffer, 1, index, rom, rom_address);
		rom_address += 8;
	}

	if ((tileset == 0x00) | (tileset == 0x07))
	{
		// Overwrite tiles 1C4-1C7 (tileset 0 and 7):
		rom_address = 0x0D8A70;
		for (index=0xC4;index<=0xC7;index++)
		{
			load_map16_tile(output_buffer, 1, index, rom, rom_address);
			rom_address += 8;
		}

		// Overwrite tiles 1C4-1C7 (tileset 0 and 7):
		rom_address = 0x0D8A90;
		for (index=0xEC;index<=0xEF;index++)
		{
			load_map16_tile(output_buffer, 1, index, rom, rom_address);
			rom_address += 8;
		}
	}

	// Tileset specific tiles (tileset 0, 7 and C):
	if ((tileset == 0x00) || (tileset == 0x07) || (tileset == 0x0C))
	{
		// Load tiles 073-0FF:
		rom_address = 0x0D8B70;
		for (index=0x73;index<=0xFF;index++)
		{
			load_map16_tile(output_buffer, 0, index, rom, rom_address);
			rom_address += 8;
		}

		// Load tiles 107-110:
		rom_address = 0x0D8FD8;
		for (index=0x07;index<=0x10;index++)
		{
			load_map16_tile(output_buffer, 1, index, rom, rom_address);
			rom_address += 8;
		}

		// Load tiles 153-16D:
		rom_address = 0x0D9028;
		for (index=0x53;index<=0x6D;index++)
		{
			load_map16_tile(output_buffer, 1, index, rom, rom_address);
			rom_address += 8;
		}
	}

	// Tileset specific tiles (tileset 1):
	if (tileset == 0x01)
	{
		// Load tiles 073-0FF:
		rom_address = 0x0DBC00;
		for (index=0x73;index<=0xFF;index++)
		{
			load_map16_tile(output_buffer, 0, index, rom, rom_address);
			rom_address += 8;
		}

		// Load tiles 107-110:
		rom_address = 0x0DC068;
		for (index=0x07;index<=0x10;index++)
		{
			load_map16_tile(output_buffer, 1, index, rom, rom_address);
			rom_address += 8;
		}

		// Load tiles 153-16D:
		rom_address = 0x0DC0B8;
		for (index=0x53;index<=0x6D;index++)
		{
			load_map16_tile(output_buffer, 1, index, rom, rom_address);
			rom_address += 8;
		}
	}

	// Tileset specific tiles (tileset 2, 6 and 8):
	if ((tileset == 0x02) || (tileset == 0x06) || (tileset == 0x08))
	{
		// Load tiles 073-0FF:
		rom_address = 0x0DC800;
		for (index=0x73;index<=0xFF;index++)
		{
			load_map16_tile(output_buffer, 0, index, rom, rom_address);
			rom_address += 8;
		}

		// Load tiles 107-110:
		rom_address = 0x0DCC68;
		for (index=0x07;index<=0x10;index++)
		{
			load_map16_tile(output_buffer, 1, index, rom, rom_address);
			rom_address += 8;
		}

		// Load tiles 153-16D:
		rom_address = 0x0DCCB8;
		for (index=0x53;index<=0x6D;index++)
		{
			load_map16_tile(output_buffer, 1, index, rom, rom_address);
			rom_address += 8;
		}
	}

	// Tileset specific tiles (tileset 3, 9, A, B and E):
	if ((tileset == 0x03) || (tileset == 0x09) || (tileset == 0x0A) || (tileset == 0x0B) || (tileset == 0x0E))
	{
		// Load tiles 073-0FF:
		rom_address = 0x0DD400;
		for (index=0x73;index<=0xFF;index++)
		{
			load_map16_tile(output_buffer, 0, index, rom, rom_address);
			rom_address += 8;
		}

		// Load tiles 107-110:
		rom_address = 0x0DD868;
		for (index=0x07;index<=0x10;index++)
		{
			load_map16_tile(output_buffer, 1, index, rom, rom_address);
			rom_address += 8;
		}

		// Load tiles 153-16D:
		rom_address = 0x0DD8B8;
		for (index=0x53;index<=0x6D;index++)
		{
			load_map16_tile(output_buffer, 1, index, rom, rom_address);
			rom_address += 8;
		}
	}

	// Tileset specific tiles (tileset 4, 5 and D):
	if ((tileset == 0x04) || (tileset == 0x05) || (tileset == 0x0D))
	{
		// Load tiles 073-0FF:
		rom_address = 0x0DE300;
		for (index=0x73;index<=0xFF;index++)
		{
			load_map16_tile(output_buffer, 0, index, rom, rom_address);
			rom_address += 8;
		}

		// Load tiles 107-110:
		rom_address = 0x0DE768;
		for (index=0x07;index<=0x10;index++)
		{
			load_map16_tile(output_buffer, 1, index, rom, rom_address);
			rom_address += 8;
		}

		// Load tiles 153-16D:
		rom_address = 0x0DE7B8;
		for (index=0x53;index<=0x6D;index++)
		{
			load_map16_tile(output_buffer, 1, index, rom, rom_address);
			rom_address += 8;
		}
	}

	return true;
}
