#ifndef FORMAT_LEVEL_OBJECT_H
#define FORMAT_LEVEL_OBJECT_H

#include "format_rom/format_enums.h"
#include "error_rom/error_definitions.h"
#include "emulate_rom/emulator.h"

struct level_object {
	unsigned char object_number = 0;
	bool new_screen = false;
	unsigned char screen_number = 0;
	unsigned int position_Y = 0;
	unsigned int position_X = 0;
	unsigned char settings_byte_1 = 0;
	unsigned char settings_byte_2 = 0;
	unsigned char settings_byte_3 = 0;
	enum_object_data_format data_format = enum_object_data_format::hw;

	bool set_type(unsigned char type);
	bool set_width(int width);
	bool set_height(int height);

	unsigned char get_type();
	unsigned int get_width();
	unsigned int get_height();

	emulation_error error = emulation_error::emulator_success;

	emulation_error decode_object(const SMW_ROM &rom, object_emulator &emulator, const level_header &header, unsigned int object_index);
};

#endif // FORMAT_LEVEL_OBJECT_H
