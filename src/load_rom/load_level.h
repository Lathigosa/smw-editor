#ifndef LOAD_LEVEL_H
#define LOAD_LEVEL_H

#include "format_rom/format_object_list.h"
//#include "main.h"
#include "utilities/queue.h"
#include "utilities/address.h"

#include "load_rom/load_rom.h"

#include "format_rom/format_level/format_level_object.h"
#include "format_rom/format_level/format_level_sprite.h"


int load_object_data(queue<level_object> &output_buffer, const SMW_ROM &rom, snes_address address, bool is_vertical_level, object_toolbox &toolbox);

int load_sprite_data(queue<level_sprite> &output_buffer, const SMW_ROM &rom, snes_address address, bool is_vertical_level);	//, sprite_toolbox &toolbox

//void decode_object_data(level_map_data &output_buffer, level_object_data &input_buffer, SMW_ROM &rom, unsigned char tileset);


#endif // LOAD_LEVEL_H
