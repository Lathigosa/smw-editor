#ifndef FORMAT_OBJECT_LIST_ITEM_H
#define FORMAT_OBJECT_LIST_ITEM_H

//#include "main.h"

#include "format_enums.h"
#include "utilities/buffer.h"
#include "error_rom/error_definitions.h"

#include "load_rom/load_rom.h"

struct object_toolbox_item
{
	enum_object_data_format format = enum_object_data_format::error_1;
	int number_of_subtypes = 0;

	buffer name;
	//buffer * subtype_name;
};

class object_toolbox
{
public:
	object_toolbox();

	object_toolbox_item items[0x40];

	rom_error load_items_format(const SMW_ROM &rom, enum_tileset current_tileset);

private:
	rom_error load_vanilla_items_format(enum_tileset current_tileset);
};

#endif // FORMAT_OBJECT_LIST_ITEM_H
