#include "main.h"

emulation_error level_object::decode_object(const SMW_ROM &rom, object_emulator &emulator, const level_header &header, unsigned int object_index)
{
	if ((rom.get_byte(0x0DA106) == 0xE2)
			&& (rom.get_byte(0x0DA107) == 0x30)) {
		// Object loading routine isn't altered by ObjecTool.
		// TODO: add support for ObjecTool.

		error = emulation_error::emulator_success;

		//qWarning() << "==== New Object:" << object_number << ", E:" << settings_byte_1 << " ====";
		if (object_number == 0x00) {
				// This is an extended object, grab routine pointer from first table in ROM:
			unsigned char address_low  = rom.get_byte(0x0DA10F + 3*settings_byte_1);
			unsigned char address_high = rom.get_byte(0x0DA110 + 3*settings_byte_1);
			unsigned char address_bank = rom.get_byte(0x0DA111 + 3*settings_byte_1);
			snes_address address = (address_low) | (address_high << 8) | (address_bank << 16);

			error = emulator.emulate_object_load(*this, address, header, object_index);

			if(address == 0x000000)
				error = emulation_error::nullpointer_object;

		} else {
			// Load the table which tells the address of the object tables for each tileset:
			unsigned char pointer_low  = rom.get_byte(0x0DA41E + 3*(header.tile_set));
			unsigned char pointer_high = rom.get_byte(0x0DA41F + 3*(header.tile_set));
			unsigned char pointer_bank = rom.get_byte(0x0DA420 + 3*(header.tile_set));
			snes_address pointer_to_table = pointer_low | (pointer_high << 8) | (pointer_bank << 16);

			// This is a normal object, grab routine pointer from second table in ROM: 0x0DA455 0x0DA456 0x0DA457
			unsigned char address_low  = rom.get_byte(pointer_to_table + 10 + 3*(object_number - 1));
			unsigned char address_high = rom.get_byte(pointer_to_table + 11 + 3*(object_number - 1));
			unsigned char address_bank = rom.get_byte(pointer_to_table + 12 + 3*(object_number - 1));
			snes_address address = (address_low) | (address_high << 8) | (address_bank << 16);

			error = emulator.emulate_object_load(*this, address, header, object_index);

			if(address == 0x000000)
				error = emulation_error::nullpointer_object;

		}

		return error;
	} else {
		if (object_number == 0x00) {
				// This is an extended object, grab routine pointer from first table in ROM:
			unsigned char address_low  = rom.get_byte(0x0DA10F + 3*settings_byte_1);
			unsigned char address_high = rom.get_byte(0x0DA110 + 3*settings_byte_1);
			unsigned char address_bank = rom.get_byte(0x0DA111 + 3*settings_byte_1);
			snes_address address = (address_low) | (address_high << 8) | (address_bank << 16);

			error = emulator.emulate_object_load(*this, 0x0DA106, header, object_index);

			//if(address == 0x000000)
			//	error = emulation_error::nullpointer_object;

		} else {
			// Load the table which tells the address of the object tables for each tileset:
			unsigned char pointer_low  = rom.get_byte(0x0DA41E + 3*(header.tile_set));
			unsigned char pointer_high = rom.get_byte(0x0DA41F + 3*(header.tile_set));
			unsigned char pointer_bank = rom.get_byte(0x0DA420 + 3*(header.tile_set));
			snes_address pointer_to_table = pointer_low | (pointer_high << 8) | (pointer_bank << 16);

			// This is a normal object, grab routine pointer from second table in ROM: 0x0DA455 0x0DA456 0x0DA457
			unsigned char address_low  = rom.get_byte(pointer_to_table + 10 + 3*(object_number - 1));
			unsigned char address_high = rom.get_byte(pointer_to_table + 11 + 3*(object_number - 1));
			unsigned char address_bank = rom.get_byte(pointer_to_table + 12 + 3*(object_number - 1));
			snes_address address = (address_low) | (address_high << 8) | (address_bank << 16);

			error = emulator.emulate_object_load(*this, address, header, object_index);

			if(address == 0x000000)
				error = emulation_error::nullpointer_object;

		}

		//return emulation_error::object_table_altered;
		return error;
	}
}
