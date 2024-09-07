#include "main.h"

int load_object_data(queue<level_object> &output_buffer, const SMW_ROM &rom, snes_address address, bool is_vertical_level, object_toolbox & toolbox)
{
	snes_address address_position = address;
	// TODO: Get header bytes

	level_object temp_object;

	address_position += 5;	// Skip header for now.

	unsigned char byte_1;
	unsigned char byte_2;
	unsigned char byte_3;
	unsigned char byte_4;
	unsigned char byte_5;

	unsigned char screen_position = 0;

	while(address_position.to_pc_format(rom.has_header) < rom.get_length())
	{
		// Retrieve next three bytes:
		byte_1 = rom.get_byte(address_position++);

		// If the end signal is here, stop decoding:
		if(byte_1 == 0xFF)
			break;

		byte_2 = rom.get_byte(address_position++);
		byte_3 = rom.get_byte(address_position++);

		if (((byte_1 & 0x60) == 0x00) && ((byte_2 & 0xF0) == 0x00) && (byte_3 == 0x00)) {			// Extended object 00 (Screen Exit)

				// Get an extra byte:
				byte_4 = rom.get_byte(address_position++);

				qWarning() << "============ SCREEN EXIT ===========";

				// TODO: Implement screen exits!
		} else if (((byte_1 & 0x60) == 0x00) && ((byte_2 & 0xF0) == 0x00) && (byte_3 == 0x01)) {	// Extended object 01 (Screen Jump)
				screen_position = byte_1 & 0x1F;

				qWarning() << "============ MOVED SCREEN POSITION TO: " << temp_object.position_Y << "";
		} else {																					// Any Normal or Extended object.

			temp_object.new_screen = (byte_1 & 0x80) >> 7;

			if (temp_object.new_screen == true)
				screen_position++;

			temp_object.object_number = ((byte_1 & 0x60) >> 1) | ((byte_2 & 0xF0) >> 4);
			temp_object.settings_byte_1 = byte_3;

			temp_object.screen_number = screen_position;

			// In vertical maps, the X and Y bits are swapped:
			if(is_vertical_level == false) {
				temp_object.position_X = (byte_2 & 0x0F) + 0x10 * temp_object.screen_number;
				temp_object.position_Y = byte_1 & 0x1F;
			} else {
				temp_object.position_Y = (byte_2 & 0x0F) + 0x10 * temp_object.screen_number;
				temp_object.position_X = byte_1 & 0x1F;
			}



			// Set the data format for each object:
			temp_object.data_format = toolbox.items[temp_object.object_number].format;

			// Test for any objects that require extra bytes to be read:
			switch (toolbox.items[temp_object.object_number].format) {
			case enum_object_data_format::unused: {
				// The object is not used, so this will result in undefined behavior when playing the game. Log a warning.

				// TODO: Add a warning system!
				break;
			}
			case enum_object_data_format::hw_tt: {
				temp_object.settings_byte_2 = rom.get_byte(address_position++);
				break;
			}
			case enum_object_data_format::hw_tt_ss: {
				temp_object.settings_byte_2 = rom.get_byte(address_position++);
				temp_object.settings_byte_3 = rom.get_byte(address_position++);
				break;
			}
			case enum_object_data_format::hw_tt_ss_ss: {
				address_position++;
				address_position++;
				address_position++;

				// TODO: add support for 3 extra byte objects!
				break;
			}
			default: {

			} }

			output_buffer.add_item(temp_object);
		}

	}
	qWarning() << "Amount of objects:" << output_buffer.get_length();

	return address_position - address;
}


int load_sprite_data(queue<level_sprite> &output_buffer, const SMW_ROM &rom, snes_address address, bool is_vertical_level)
{
	snes_address address_position = address;
	// TODO: Get header bytes

	address_position += 1;	// Skip header for now.

	while(address_position.to_pc_format(rom.has_header) < rom.get_length())
	{
		level_sprite temp_sprite;

		// Retrieve next three bytes:
		unsigned char byte_1 = rom.get_byte(address_position++);

		// If the end signal is here, stop decoding:
		if(byte_1 == 0xFF)
			break;

		unsigned char byte_2 = rom.get_byte(address_position++);
		unsigned char byte_3 = rom.get_byte(address_position++);

		temp_sprite.screen_number = (byte_2 & 0x0F) | ((byte_1 & 0x02) << 3);
		temp_sprite.extra_bits = (byte_1 & 0x0C) >> 2;
		temp_sprite.sprite_number = byte_3;

		// In vertical maps, the X and Y bits are swapped:
		if(is_vertical_level == false) {
			temp_sprite.position_x = ((byte_2 & 0xF0) >> 4) + 0x10 * temp_sprite.screen_number;
			temp_sprite.position_y = ((byte_1 & 0xF0) >> 4) | ((byte_1 & 0x01) << 4);
		} else {
			temp_sprite.position_y = ((byte_2 & 0xF0) >> 4) + 0x10 * temp_sprite.screen_number;
			temp_sprite.position_x = ((byte_1 & 0xF0) >> 4) | ((byte_1 & 0x01) << 4);
		}

		output_buffer.add_item(temp_sprite);
	}
	qWarning() << "Amount of sprites:" << output_buffer.get_length();

	return address_position - address;
}
