#include "main.h"

#include "save_rom/encode_level.h"

// =============== LEVEL MAP DATA =============== //

level_map_data::level_map_data(unsigned int size_x, unsigned int size_y)
{
	map_size_x = size_x;
	map_size_y = size_y;
	buffer_data = new map_data[size_x * size_y];		// The size of any map is 512 blocks wide and 32 blocks tall.
}

level_map_data::level_map_data()
{
	map_size_x = 0;
	map_size_y = 0;
	buffer_data = NULL;
}

level_map_data::level_map_data( const level_map_data& other ) :
	map_size_x( other.map_size_x ),
	map_size_y( other.map_size_y )
{
	if(buffer_data != NULL)
		delete[] buffer_data;
	buffer_data = new map_data[map_size_x * map_size_y];

	unsigned int a;
	for(a = 0;a < (map_size_x * map_size_y); a++)
	{
		buffer_data[a] = other.buffer_data[a];
	}
}

level_map_data& level_map_data::operator=( const level_map_data& other ) {
	map_size_x = other.map_size_x;
	map_size_y = other.map_size_y;

	if(buffer_data != NULL)
		delete[] buffer_data;
	buffer_data = new map_data[map_size_x * map_size_y];

	unsigned int a;
	for(a = 0;a < (map_size_x * map_size_y); a++)
	{
		buffer_data[a] = other.buffer_data[a];
	}

	return *this;
}

level_map_data::~level_map_data()
{
	if(buffer_data != NULL)
		delete[] buffer_data;						// Clean up junk when destroyed.
}

// Clears the data of the map and sets the MAP16 number to 25.
void level_map_data::clear_data()
{
	if(buffer_data == NULL)
		return;

	int a;
	for (a=0; a < map_size_x*map_size_y; a++) {
		buffer_data[a].map16_number_high = map16_clear_tile_high;
		buffer_data[a].map16_number_low = map16_clear_tile_low;
		buffer_data[a].owner = 0;
	}
}

void level_map_data::decodeObject(level_object &object)
{

}

unsigned char level_map_data::getMapDataLow(unsigned int X, unsigned int Y)
{
	if(X > (map_size_x - 1))
		return 0;

	if(Y > (map_size_y - 1))
		return 0;

	return buffer_data[X * map_size_y + Y].map16_number_low;
}

unsigned char level_map_data::getMapDataHigh(unsigned int X, unsigned int Y)
{
	if(X > (map_size_x - 1))
		return 0;

	if(Y > (map_size_y - 1))
		return 0;

	return buffer_data[X * map_size_y + Y].map16_number_high;
}

unsigned int level_map_data::getMapDataOwner(unsigned int X, unsigned int Y)
{
	if(X > (map_size_x - 1))
		return 0;

	if(Y > (map_size_y - 1))
		return 0;

	return buffer_data[X * map_size_y + Y].owner;
}

bool level_map_data::setMapData(unsigned int X, unsigned int Y, unsigned char page, unsigned char byte, unsigned int owner)
{
	if(X > (map_size_x - 1))
		return false;

	if(Y > (map_size_y - 1))
		return false;

	buffer_data[X * map_size_y + Y].map16_number_high = page;
	buffer_data[X * map_size_y + Y].map16_number_low = byte;
	buffer_data[X * map_size_y + Y].owner = owner;

	return true;
}

bool level_map_data::setMapDataLow(unsigned int X, unsigned int Y, unsigned char byte, unsigned int owner)
{
	if(X > (map_size_x - 1))
		return false;

	if(Y > (map_size_y - 1))
		return false;

	buffer_data[X * map_size_y + Y].map16_number_low = byte;
	buffer_data[X * map_size_y + Y].owner = owner;

	return true;
}

bool level_map_data::setMapDataHigh(unsigned int X, unsigned int Y, unsigned char page, unsigned int owner)
{
	if(X > (map_size_x - 1))
		return false;

	if(Y > (map_size_y - 1))
		return false;

	buffer_data[X * map_size_y + Y].map16_number_high = page;
	buffer_data[X * map_size_y + Y].owner = owner;

	return true;
}

unsigned int level_map_data::getMapSizeX() const
{
	return map_size_x;
}

unsigned int level_map_data::getMapSizeY() const
{
	return map_size_y;
}

// ============================================================================

level_data::level_data()
{

}

level_data::~level_data() {

}

rom_error level_data::save_level(SMW_ROM & rom, unsigned int level_number)
{
	// TODO: Save layer 2 as well!




	rats_tag tag;
	tag.location = address_of_level[0].to_pc_format(rom.has_header) - 16;

	if(rom.deallocate_rats(tag) == false)
		qWarning() << "Could not deallocate.";

	rats_block saved_block;


	// Layer 1:
	unsigned int internal_location_of_layer_1 = 0;
	{
		buffer encoded_level = encode_level(object_list[0], header);
		internal_location_of_layer_1 = saved_block.append_partition(encoded_level, partition_type::level_layer_1);
		byte_size_of_layer[0] = encoded_level.get_length();
	}
	// Layer 2:
	unsigned int internal_location_of_layer_2 = 0;
	if(header.is_layer2_level == true)
	{
		buffer encoded_level = encode_level(object_list[1], header);
		internal_location_of_layer_2 = saved_block.append_partition(encoded_level, partition_type::level_layer_2);
		byte_size_of_layer[1] = encoded_level.get_length();
	}

	tag = rom.allocate_rats(saved_block);

	if(tag.location.is_valid() == false)
		return rom_error::no_freespace;

	tag.location += 8;

	pc_address layer_1_location = internal_location_of_layer_1 + tag.location;
	address_of_level[0] = layer_1_location.to_snes_format(rom.has_header);

	// Set the address of the current level (layer 1) in the table:1
	rom.set_byte(0x05E000 + 0 + 3 * level_number, address_of_level[0].low());
	rom.set_byte(0x05E000 + 1 + 3 * level_number, address_of_level[0].high());
	rom.set_byte(0x05E000 + 2 + 3 * level_number, address_of_level[0].bank());

	// Set the address of the current level (layer 2) in the table:
	if(header.is_layer2_level == true)
	{
		pc_address layer_2_location = internal_location_of_layer_2 + tag.location;
		tag.location += internal_location_of_layer_2 - 8;
		address_of_level[1] = layer_2_location.to_snes_format(rom.has_header);

		rom.set_byte(0x05E600 + 0 + 3 * level_number, address_of_level[1].low());
		rom.set_byte(0x05E600 + 1 + 3 * level_number, address_of_level[1].high());
		rom.set_byte(0x05E600 + 2 + 3 * level_number, address_of_level[1].bank());
	}

	return rom_error::success;

}

int level_data::decode_object_data(const SMW_ROM &rom, unsigned int layer)
{
	if(layer >= 2)
		return 1;

	//if ((rom.get_byte(0x0DA106) == 0xE2)
	//		&& (rom.get_byte(0x0DA107) == 0x30)) {
		// Object loading routine isn't altered by ObjecTool.
		// TODO: add support for ObjecTool.

		object_emulator new_emulator(rom, 0, 0);

		unsigned int error_count = 0;	// Counts the amount of failed objects.

		emulation_error get_error = emulation_error::emulator_success;

		int a;
		for (a=0; a<object_list[layer].get_length(); a++) {
			get_error = object_list[layer].get_item(a).decode_object(rom, new_emulator, header, a + 1);

			// TODO: add better error report system.
			if (get_error != emulation_error::emulator_success) {
				error_count++;
				qWarning() << "Object emulator (" << a << ") failed with error " << (int)get_error;
			}
		}

		new_emulator.update_level_map(level_map[layer]);

		return error_count;
	//} else {
	//	qWarning() << "ERROR: This ROM seems to be altered with ObjecTool. This is currently unsupported.";
	//	return emulation_error::object_table_altered;
	//}
}

int level_data::load_level(unsigned int level_number, const SMW_ROM & rom)
{
	if (level_number >= 512)
		return -1;	// TODO: add error message!

	// Get the address of currently loaded level (layer 1):
	{
		unsigned char address_low  = rom.get_byte(0x05E000 + 0 + 3 * level_number);
		unsigned char address_high = rom.get_byte(0x05E000 + 1 + 3 * level_number);
		unsigned char address_bank = rom.get_byte(0x05E000 + 2 + 3 * level_number);
		address_of_level[0] = (unsigned int)(address_low | (address_high << 8) | (address_bank << 16));
	}

	// Get the address of currently loaded level (layer 2):
	{
		unsigned char address_low  = rom.get_byte(0x05E600 + 0 + 3 * level_number);
		unsigned char address_high = rom.get_byte(0x05E600 + 1 + 3 * level_number);
		unsigned char address_bank = rom.get_byte(0x05E600 + 2 + 3 * level_number);
		address_of_level[1] = (unsigned int)(address_low | (address_high << 8) | (address_bank << 16));
	}

	// Get the address of currently loaded level (sprites):
	{
		unsigned char address_low  = rom.get_byte(0x05EC00 + 0 + 2 * level_number);
		unsigned char address_high = rom.get_byte(0x05EC00 + 1 + 2 * level_number);
		unsigned char address_bank = rom.get_byte(0x05E000 + 2 + 2 * level_number);
		address_bank = 0x07;
		address_of_sprites = (unsigned int)(address_low | (address_high << 8) | (address_bank << 16));
	}

	// Get primary header information:
	{
		unsigned char byte_1 = rom.get_byte(address_of_level[0] + 0);
		unsigned char byte_2 = rom.get_byte(address_of_level[0] + 1);
		unsigned char byte_3 = rom.get_byte(address_of_level[0] + 2);
		unsigned char byte_4 = rom.get_byte(address_of_level[0] + 3);
		unsigned char byte_5 = rom.get_byte(address_of_level[0] + 4);

		header.BG_palette = (byte_1 & 0xE0) >> 5;
		header.amount_of_screens = (byte_1 & 0x1F);
		header.BG_color = (byte_2 & 0xE0) >> 5;
		header.level_mode = (byte_2 & 0x1F);
		header.layer_3_priority = (byte_3 & 0x80);
		header.music = (byte_3 & 0x70) >> 4;
		header.sprite_set = (byte_3 & 0x0F);
		header.time_limit = (byte_4 & 0xC0) >> 6;
		header.sprite_palette = (byte_4 & 0x38) >> 3;
		header.FG_palette = (byte_4 & 0x07);
		header.item_memory = (byte_5 & 0xC0) >> 6;
		header.vertical_scroll = (byte_5 & 0x30) >> 4;
		header.tile_set = (byte_5 & 0x0F);

		// Get derived level mode information:
		header.is_vertical_level = rom.get_byte(0x058417 + header.level_mode) & 0x01;	// Get the value from the lowest bit from a byte in the level mode table at 0x058417.

		// HARDCODE:
		if(header.level_mode == 0x01
				|| header.level_mode == 0x02
				|| header.level_mode == 0x07
				|| header.level_mode == 0x08
				|| header.level_mode == 0x0F
				|| header.level_mode == 0x1F)
			header.is_layer2_level = true;
		else
			header.is_layer2_level = false;

		//058C85: Which level mode will use different palette for layer 2.
	}

	// Get secondary header information:
	{
		unsigned char byte_1 = rom.get_byte(0x05F000 + level_number);
		unsigned char byte_2 = rom.get_byte(0x05F200 + level_number);
		unsigned char byte_3 = rom.get_byte(0x05F400 + level_number);
		unsigned char byte_4 = rom.get_byte(0x05F600 + level_number);

		header.layer_2_scroll_settings = (byte_1 & 0xF0) >> 4;
		header.level_entrance_Y_position = (byte_1 & 0x0F);
		header.layer_3_settings = (byte_2 & 0xC0) >> 6;
		header.level_entrance_type = (byte_2 & 0x38) >> 3;
		header.level_entrance_X_position = (byte_2 & 0x07);
		header.level_entrance_midway_screen = (byte_3 & 0xF0) >> 4;
		header.level_entrance_FG_init_position = (byte_3 & 0x0C) >> 2;
		header.level_entrance_BG_init_position = (byte_3 & 0x03);
		header.disable_no_yoshi_intro = (byte_4 & 0x80);
		header.unknown_vertical_position_flag = (byte_4 & 0x40);
		header.vertical_position_flag = (byte_4 & 0x20);
		header.level_entrance_screen_number = (byte_4 & 0x1F);
	}

	// First, clear the level map and objects to prevent cluttering:
	if (header.is_vertical_level == false )
	{
		level_map[0] = level_map_data(512, 27);
		level_map[1] = level_map_data(512, 27);
	}
	else
	{
		level_map[0] = level_map_data(32, 512);
		level_map[1] = level_map_data(32, 512);
	}

	object_list[0].clear_data();
	object_list[1].clear_data();
	sprite_list.clear_data();

	// Load the toolbox data in order to know what byte size each object is:
	toolbox.load_items_format(rom, (enum_tileset)header.tile_set);
	toolbox_sprites.load_items_format(rom, (enum_tileset)header.tile_set);

	// Load all the objects:
	byte_size_of_layer[0] = load_object_data(object_list[0], rom, address_of_level[0], header.is_vertical_level, toolbox);
	byte_size_of_layer[1] = load_object_data(object_list[1], rom, address_of_level[1], header.is_vertical_level, toolbox);
	byte_size_of_sprites = load_sprite_data(sprite_list, rom, address_of_sprites, header.is_vertical_level);

	return update_level(rom);
}

int level_data::update_level(const SMW_ROM &rom, unsigned int layer)
{
	if(layer >= 2)
		return 1;

	level_map[layer].clear_data();
	return decode_object_data(rom, layer);
}

int level_data::update_level(const SMW_ROM &rom)
{
	level_map[0].clear_data();
	level_map[1].clear_data();

	int amount_of_errors_1 = decode_object_data(rom, 0);
	int amount_of_errors_2 = decode_object_data(rom, 1);

	if(amount_of_errors_1 < 0)
		return amount_of_errors_1;

	if(amount_of_errors_2 < 0)
		return amount_of_errors_2;

	return amount_of_errors_1 + amount_of_errors_2;
}

void level_data::move_object_x(unsigned int layer, unsigned int index, int position_x)
{
	if(position_x < 0)
		position_x = 0;

	level_object new_object = object_list[layer].get_item(index);
	new_object.position_X = position_x;

	if(header.is_vertical_level == false)
		new_object.screen_number = position_x / 0x10;

	object_list[layer].set_item(index, new_object);
}

void level_data::move_object_y(unsigned int layer, unsigned int index, int position_y)
{
	if(position_y < 0)
		position_y = 0;

	level_object new_object = object_list[layer].get_item(index);
	new_object.position_Y = position_y;

	if(header.is_vertical_level == true)
		new_object.screen_number = position_y / 0x10;

	object_list[layer].set_item(index, new_object);
}
