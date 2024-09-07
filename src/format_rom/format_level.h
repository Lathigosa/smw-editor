#ifndef FORMAT_LEVEL_H
#define FORMAT_LEVEL_H

#include "load_rom/load_rom.h"
#include "utilities/queue.h"
#include "format_level/format_level_sprite.h"
#include "format_level/format_level_object.h"
#include "format_rom/format_object_list.h"

struct level_header {
	// Primary layer header:
	unsigned char BG_palette;
	unsigned char amount_of_screens;
	unsigned char BG_color;
	unsigned char level_mode;
	bool layer_3_priority;
	unsigned char music;
	unsigned char sprite_set;
	unsigned char time_limit;
	unsigned char sprite_palette;
	unsigned char FG_palette;
	unsigned char item_memory;
	unsigned char vertical_scroll;
	unsigned char tile_set;

	// Secondary header:
	unsigned char layer_2_scroll_settings;
	unsigned char level_entrance_Y_position;
	unsigned char layer_3_settings;
	unsigned char level_entrance_type;
	unsigned char level_entrance_X_position;
	unsigned char level_entrance_midway_screen;
	unsigned char level_entrance_FG_init_position;
	unsigned char level_entrance_BG_init_position;
	bool disable_no_yoshi_intro;
	bool unknown_vertical_position_flag;
	bool vertical_position_flag;
	unsigned char level_entrance_screen_number;

	// Sprites:
	unsigned char sprite_memory;
	bool sprite_buoyancy;
	bool sprite_buoyancy_no_layer_2_interaction;

	// Some readonly derivatives:
	bool is_vertical_level;
	bool is_layer2_level;
};


struct map_data {
	unsigned char map16_number_low;
	unsigned char map16_number_high;

	unsigned int owner;					// The object index this block belongs to.
};

class level_map_data {
public:
	level_map_data(unsigned int size_x, unsigned int size_y);
	level_map_data();

	level_map_data( const level_map_data& other );
	level_map_data& operator=( const level_map_data& other );

	~level_map_data();

	void clear_data();

	unsigned char map16_clear_tile_low = 0x25;
	unsigned char map16_clear_tile_high = 0x00;

	void decodeObject(level_object &object);
	unsigned char getMapDataLow(unsigned int X, unsigned int Y);
	unsigned char getMapDataHigh(unsigned int X, unsigned int Y);
	unsigned int getMapDataOwner(unsigned int X, unsigned int Y);
	bool setMapData(unsigned int X, unsigned int Y, unsigned char page, unsigned char byte, unsigned int owner);
	bool setMapDataLow(unsigned int X, unsigned int Y, unsigned char byte, unsigned int owner);
	bool setMapDataHigh(unsigned int X, unsigned int Y, unsigned char page, unsigned int owner);

	unsigned int getMapSizeX() const;
	unsigned int getMapSizeY() const;

private:
	map_data * buffer_data = NULL;

	unsigned int map_size_x;
	unsigned int map_size_y;
};

class level_data {
public:
	level_data();
	~level_data();

	int load_level(unsigned int level_number, const SMW_ROM & rom);
	int update_level(const SMW_ROM &rom);
	int update_level(const SMW_ROM &rom, unsigned int layer);

	rom_error save_level(SMW_ROM & rom, unsigned int level_number);

	void move_object_x(unsigned int layer, unsigned int index, int position_x);
	void move_object_y(unsigned int layer, unsigned int index, int position_y);

	level_header header;
	level_map_data level_map[2];						// The level block tilemap, one for each layer.
	queue<level_object> object_list[2];					// The level object list, one for each layer.

	queue<level_sprite> sprite_list;					// The level sprite list.


	snes_address address_of_level[2];
	unsigned int byte_size_of_layer[2];

	snes_address address_of_sprites;
	unsigned int byte_size_of_sprites;

	int decode_object_data(const SMW_ROM &rom, unsigned int layer);

	object_toolbox toolbox;
	sprite_toolbox toolbox_sprites;
//private:

};

//class level_data {
//public:

//	level_header header;
//	level_map_data layer_maps[2];					// The level block tilemap, one for each layer.
//	queue<level_object> object_lists[2];			// The level object list, one for each layer.

//	object_toolbox toolbox;

//	queue<level_sprite> sprite_list;				// The level sprite list.
//};

#endif // FORMAT_LEVEL_H
