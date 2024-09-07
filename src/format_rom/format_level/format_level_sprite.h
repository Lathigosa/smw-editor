#ifndef FORMAT_LEVEL_SPRITE_H
#define FORMAT_LEVEL_SPRITE_H

#include "format_rom/format_enums.h"
#include "utilities/buffer.h"
#include "error_rom/error_definitions.h"
#include "load_rom/load_rom.h"

struct sprite_toolbox_item
{

	//int number_of_subtypes = 0;

	buffer name;

	unsigned int opengl_tilemap = 0;
	int size_x = 2;
	int size_y = 2;

	int displacement_x = 0;		// Displacement in pixels.
	int displacement_y = 0;		// Displacement in pixels.

	//buffer * subtype_name;
};

class sprite_toolbox
{
public:
	sprite_toolbox_item items[0x100];

	rom_error load_items_format(const SMW_ROM &rom, enum_tileset current_tileset);

private:
	rom_error load_vanilla_items_format(enum_tileset current_tileset);
};

struct level_sprite
{
	unsigned int position_y = 0;
	unsigned int position_x = 0;
	unsigned char extra_bits = 0;
	unsigned char screen_number = 0;

	unsigned char sprite_number = 0;
};

#endif // FORMAT_LEVEL_SPRITE_H
