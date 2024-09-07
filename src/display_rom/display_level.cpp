#include "main.h"

// This function converts one map16 tile to 8x8 graphics
// tiles, by looking up the tiles in the map16 data. It puts
// the tiles in the level map texture. This should be called
// each time map16 is updated or if the level is altered.
void display_level_tile(const map16_map &map16, unsigned char map16_high, unsigned char map16_low, unsigned int x_position, unsigned int y_position, GLuint texture)
{
	glBindTexture(GL_TEXTURE_2D, texture);

	unsigned int a;
	unsigned int b;	// For fixing data format.

	unsigned char data[4*4];

	for(a=0;a<4;a++)
	{
		// Fix data format (flip the tiles where necessary, because of how OpenGL works) (mirror along the axis x=y):
		b = a;
		if (a == 1 || a == 2)
			b = 3 - a;

		unsigned char offset_x;
		unsigned char offset_y;

		if (map16.getPageCount() <= map16_high) {
			return;	// TODO: send error message
		}

		offset_x = ((map16.getBlock(map16_high, map16_low).gfx_tiles[a].high_byte & 0x02) >> 1) * 16;
		offset_y = ((map16.getBlock(map16_high, map16_low).gfx_tiles[a].high_byte & 0x01)) * 16;

		offset_x +=  (map16.getBlock(map16_high, map16_low).gfx_tiles[a].low_byte & 0x0F);			// The last four bits of the tile is the x offset.
		offset_y += ((map16.getBlock(map16_high, map16_low).gfx_tiles[a].low_byte & 0xF0) >> 4);	// The first four bits of the tile is the y offset.

		data[b*4 + 0] = offset_x;
		data[b*4 + 1] = offset_y;
		if (map16.getBlock(map16_high, map16_low).gfx_tiles[a].flip_y == true)
			data[b*4 + 2] = 255 - map16.getBlock(map16_high, map16_low).gfx_tiles[a].flip_x * 64;
		else
			data[b*4 + 2] = map16.getBlock(map16_high, map16_low).gfx_tiles[a].flip_x * 64;
		data[b*4 + 3] = map16.getBlock(map16_high, map16_low).gfx_tiles[a].palette;
	}

	glTexSubImage2D(GL_TEXTURE_2D,
					0,
					x_position * 2,
					y_position * 2,
					2,												// Size of each tile is 2 by 2 8x8 tiles.
					2,
					GL_RGBA,
					GL_UNSIGNED_BYTE,
					reinterpret_cast<void*>(data));
}

// This function converts the level map data to 8x8 graphics
// tiles, by looking up the tiles in the map16 data. It puts
// the tiles in the level map texture. This should be called
// each time map16 is updated or if the level is altered. It
// displays the entire level_map_data map.
emulation_error display_level_map(const map16_map &map16, level_map_data &level_map, GLuint texture)
{
	return display_level_map(map16, level_map, texture, 0, 0, level_map.getMapSizeX(), level_map.getMapSizeY());
}

// This function converts the level map data to 8x8 graphics
// tiles, by looking up the tiles in the map16 data. It puts
// the tiles in the level map texture. This should be called
// each time map16 is updated or if the level is altered. It
// displays only a section of the level_map_data map.
emulation_error display_level_map(const map16_map &map16, level_map_data &level_map, GLuint texture, unsigned int x_min, unsigned int y_min, unsigned int x_max, unsigned int y_max)
{
	if(x_max > level_map.getMapSizeX())
		x_max = level_map.getMapSizeX();

	if(y_max > level_map.getMapSizeY())
		y_max = level_map.getMapSizeY();

	if(x_min >= x_max || y_min >= y_max)
		return emulation_error::tilemap_out_of_bounds;

	emulation_error error = emulation_error::emulator_success;


	unsigned int y_block;
	unsigned int x_block;

	unsigned char * full_data = new unsigned char[(y_max - y_min) * (x_max - x_min) * 4 * 4];

	for (y_block=y_min; y_block<y_max; y_block++)
	{
		for (x_block=x_min; x_block<x_max; x_block++)
		{
			unsigned int x = (x_block - x_min) * 2;
			unsigned int y = (y_block - y_min) * 2;



			//qWarning() << "Block at (" << x << ", " << y << ") has a high byte of " << (int)level_map.getMapData(x, y)->map16_number_high << " and a low byte of " << (int)level_map.getMapData(x, y)->map16_number_low << " ";

			//display_level_tile(map16,
			//				   level_map.getMapDataHigh(x, y),
			//				   level_map.getMapDataLow(x, y),
			//				   x,
			//				   y,
			//				   texture);

			// START OF MAP16 TILE

			unsigned char map16_high = level_map.getMapDataHigh(x_block, y_block);
			unsigned char map16_low = level_map.getMapDataLow(x_block, y_block);

			unsigned int a;

			unsigned char data[4*4];

			for(a=0;a<4;a++)
			{
				// Fix data format (flip the tiles where necessary, because of how OpenGL works) (mirror along the axis x=y):
				int x_shift;
				int y_shift;

				if (a == 1 || a == 3)
					y_shift = 1;
				else
					y_shift = 0;

				if (a == 2 || a == 3)
					x_shift = 1;
				else
					x_shift = 0;


				unsigned char offset_x;
				unsigned char offset_y;

				if (map16.getPageCount() <= map16_high) {
					error = emulation_error::map16_out_of_range;	// TODO: send error message
					//map16_high = map16.getPageCount() - 1;
				}

				offset_x = ((map16.getBlock(map16_high, map16_low).gfx_tiles[a].high_byte & 0x02) >> 1) * 16;
				offset_y = ((map16.getBlock(map16_high, map16_low).gfx_tiles[a].high_byte & 0x01)) * 16;

				offset_x +=  (map16.getBlock(map16_high, map16_low).gfx_tiles[a].low_byte & 0x0F);			// The last four bits of the tile is the x offset.
				offset_y += ((map16.getBlock(map16_high, map16_low).gfx_tiles[a].low_byte & 0xF0) >> 4);	// The first four bits of the tile is the y offset.

				data[a*4 + 0] = offset_x;
				data[a*4 + 1] = offset_y;
				if (map16.getBlock(map16_high, map16_low).gfx_tiles[a].flip_y == true)
					data[a*4 + 2] = 255 - map16.getBlock(map16_high, map16_low).gfx_tiles[a].flip_x * 64;
				else
					data[a*4 + 2] = map16.getBlock(map16_high, map16_low).gfx_tiles[a].flip_x * 64;
				data[a*4 + 3] = map16.getBlock(map16_high, map16_low).gfx_tiles[a].palette;

				full_data[4*((y + y_shift) * ((x_max - x_min) * 2) + (x + x_shift))        + 0] = data[a*4 + 0];
				full_data[4*((y + y_shift) * ((x_max - x_min) * 2) + (x + x_shift))        + 1] = data[a*4 + 1];
				full_data[4*((y + y_shift) * ((x_max - x_min) * 2) + (x + x_shift))        + 2] = data[a*4 + 2];
				full_data[4*((y + y_shift) * ((x_max - x_min) * 2) + (x + x_shift))        + 3] = data[a*4 + 3];
			}
			// END OF MAP16 TILE


		}
	}


	glBindTexture(GL_TEXTURE_2D, texture);

	glTexSubImage2D(GL_TEXTURE_2D,
					0,
					x_min * 2,
					y_min * 2,
					(x_max - x_min) * 2,												// Size of each tile is 2 by 2 8x8 tiles.
					(y_max - y_min) * 2,
					GL_RGBA,
					GL_UNSIGNED_BYTE,
					reinterpret_cast<void*>(full_data));

	delete[] full_data;

	return error;
}

emulation_error prepare_display_level_sprites(sprite_toolbox &toolbox)
{
	for(unsigned int a=0; a<0x100; a++)
	{
		if(toolbox.items[a].opengl_tilemap == 0)
			glGenTextures(1, &toolbox.items[a].opengl_tilemap);

		glBindTexture(GL_TEXTURE_2D, toolbox.items[a].opengl_tilemap);

		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, toolbox.items[a].size_x, toolbox.items[a].size_y, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);

		// Blocky filtering:
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

		unsigned char data[4*4] = {0x00, 0x00, 0x00, 0x00,
								   0x01, 0x00, 0x00, 0x00,
								   0x00, 0x01, 0x00, 0x00,
								   0x01, 0x01, 0x00, 0x00};

		glTexSubImage2D(GL_TEXTURE_2D,
						0,
						0 * 2,
						0 * 2,
						2,												// Size of each tile is 2 by 2 8x8 tiles.
						2,
						GL_RGBA,
						GL_UNSIGNED_BYTE,
						reinterpret_cast<void*>(data));
	}

	return emulation_error::emulator_success;
}

emulation_error display_level_sprites(const queue<level_sprite> &sprite_list, queue<opengl_sprite> &render_list, const sprite_toolbox &toolbox)
{
	render_list.clear_data();

	for(unsigned int a=0; a<sprite_list.get_length(); a++)
	{
		opengl_sprite temp_sprite;
		temp_sprite.tilemap = toolbox.items[sprite_list.get_item(a).sprite_number].opengl_tilemap;
		temp_sprite.pos_x = sprite_list.get_item(a).position_x * 16 + toolbox.items[sprite_list.get_item(a).sprite_number].displacement_x;
		temp_sprite.pos_y = sprite_list.get_item(a).position_y * 16 + toolbox.items[sprite_list.get_item(a).sprite_number].displacement_y;
		temp_sprite.size_x = toolbox.items[sprite_list.get_item(a).sprite_number].size_x;
		temp_sprite.size_y = toolbox.items[sprite_list.get_item(a).sprite_number].size_y;

		render_list.add_item(temp_sprite);
	}

	return emulation_error::emulator_success;
}
