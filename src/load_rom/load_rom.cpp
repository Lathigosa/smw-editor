#include "main.h"


SMW_ROM::SMW_ROM()
{
	is_name_set = false;
	buffer_exists = false;
}

SMW_ROM::~SMW_ROM()
{

}

bool SMW_ROM::is_open() const {
	return buffer_exists;
}

// This changes the path to the currently edited file. It does not actually open the file,
//   it just sets the filename. Returns "file_ok" if succeeded.
file_error SMW_ROM::set_filename(const char * filename)
{
	// TODO: Make a proper strcpy function:
	opened_filename = filename;
	is_name_set = true;
	return file_error::file_ok;
}

// This reloads the contents of the internal buffer. Call each time before loading stuff.
file_error SMW_ROM::load_rom()
{
	// Check if OpenFilename is valid:
	if (is_name_set == false)								// If OpenFilename isn't specified, don't continue.
		return file_error::file_namenotset;

	// Load the new buffer from file:
	buffer_exists = true;
	rom_buffer = load_file((char*)opened_filename.getBuffer());

	// Check if loading the file succeeded:
	if (rom_buffer.get_length() == 0) {
		buffer_exists = false;
		return file_error::file_genericerror;
	}

	// Test the file for header:
	qWarning() << "The length of the file is: " << rom_buffer.get_length() << rom_buffer.get_length();
	has_header = test_header(rom_buffer.getBuffer());						// Test for header, <true> if ROM has a 512 byte header.
	if (test_header_genuine(rom_buffer.getBuffer()) == false) {				// Test for genuinity, <true> if ROM is an SMW rom.
		// File not SMW, unload from memory and reset:
		buffer_exists = false;
		return file_error::file_notsmw;
	}
	return file_error::file_ok;
}

file_error SMW_ROM::save_rom()
{
	// Check if OpenFilename is valid:
	if (is_name_set == false)								// If OpenFilename isn't specified, don't continue.
		return file_error::file_namenotset;

	// Save the file:
	if(save_file((char*)opened_filename.getBuffer(), rom_buffer) == true)
		return file_error::file_ok;

	return file_error::file_genericerror;
}

unsigned char SMW_ROM::get_byte_pc(unsigned int pc_address) const
{
	if(buffer_exists == false)
		return 0x00;

	if(pc_address >= rom_buffer.get_length())
		return 0x00;

	return rom_buffer[pc_address];
}

unsigned char SMW_ROM::get_byte(snes_address address) const
{
	if(address.is_valid_rom() == false)
		return 0x00;

	//if((unsigned int)address.to_pc_format(has_header) >= length)
	//	return 0x00;

	return rom_buffer[(unsigned int)address.to_pc_format(has_header)];
}

void SMW_ROM::set_byte_pc(unsigned int pc_address, unsigned char data)
{
	if(pc_address >= rom_buffer.get_length())
		return;

	rom_buffer[pc_address] = data;
}

void SMW_ROM::set_byte(snes_address address, unsigned char data)
{
	if(address.is_valid_rom() == false)
		return;

	//if((unsigned int)address.to_pc_format(has_header) >= length)
	//	return;

	rom_buffer[(unsigned int)address.to_pc_format(has_header)] = data;
}

void SMW_ROM::expand_rom(rom_size size)
{
	unsigned char current_size = get_byte(0x00FFD7);

	if((unsigned char)size > current_size)
	{
		set_byte(0x00FFD7, (unsigned char)size);
		buffer old_buffer = rom_buffer;
		rom_buffer.reset_size((0x400 << (unsigned char)size) + has_header * 512);
		rom_buffer.clear_buffer();
		rom_buffer.import_buffer_at(old_buffer, 0x00);
	}
}

snes_address SMW_ROM::find_unprotected_freespace(int data_size, unsigned char bank, unsigned char empty_byte)
{
	bool has_found = false;
	snes_address starting_address = (bank << 16) + 0x8000;
	pc_address current_address = starting_address.to_pc_format(has_header);

	int found_chunk = 0;

	while(has_found == false)
	{
		if(rom_buffer[current_address] == empty_byte)
		{
			found_chunk++;
			if(found_chunk == data_size)
				has_found = true;

		} else {
			found_chunk = 0;
		}
		current_address++;
	}

	pc_address found_pc_address = current_address - found_chunk;
	snes_address found_snes_address = found_pc_address.to_snes_format(has_header);

	if((current_address.to_snes_format(has_header).bank() != bank) || (found_snes_address.bank() != bank))
		return 0xFFFFFFFF;

	return found_snes_address;
}

void SMW_ROM::scan_for_rats_tags()
{
	// Start at ROM bank 0x10:
	snes_address starting_address = 0x108000;

	// Reallocate the RATS tag list:
	rats_tag_list.clear_data();

	// Fill the list:
	for(int a=starting_address.to_pc_format(has_header); a<rom_buffer.get_length(); a++)
	{
		// If this address contains a RATS tag:
		if(    rom_buffer[a + 0] == 'S'
			&& rom_buffer[a + 1] == 'T'
			&& rom_buffer[a + 2] == 'A'
			&& rom_buffer[a + 3] == 'R'
			&& (unsigned char)rom_buffer[a + 4] == (unsigned char)~rom_buffer[a + 6]
			&& (unsigned char)rom_buffer[a + 5] == (unsigned char)~rom_buffer[a + 7] )
		{
			rats_tag new_rats_tag;
			new_rats_tag.allocation_size = (rom_buffer[a + 4] | (rom_buffer[a + 5]) << 8) + 1;
			new_rats_tag.location = a;

			rats_tag_list.add_item(new_rats_tag);
		}
	}
}

pc_address SMW_ROM::find_rats_freespace(int data_size)
{
	scan_for_rats_tags();

	bool has_found = false;
	snes_address starting_address = 0x108000;
	pc_address current_address = starting_address.to_pc_format(has_header);

	int found_chunk = 0;

	pc_address start_of_chunk = current_address;

	while(has_found == false)
	{

		// Iterate and check each byte if it is empty:
		if(rom_buffer[current_address] == 0x00)
		{
			found_chunk++;

			// Once we found an empty chunk large enough:
			if(found_chunk == data_size)
			{
				has_found = true;

				// If the found chunk crosses banks, restart looking from the start of the next bank:
				if((current_address.to_snes_format(has_header).bank() != start_of_chunk.to_snes_format(has_header).bank()))
				{
					starting_address = (current_address.to_snes_format(has_header).bank() << 16) + 0x8000;
					current_address = starting_address.to_pc_format(has_header);
					has_found = false;
					start_of_chunk = current_address;
					found_chunk = 0;
				}

				// Check if it is not in a RATS protected area:
				for(unsigned int a=0; a<rats_tag_list.get_length(); a++)
				{
					// If the found freespace is within a RATS protected area, restart looking at the end of the area:
					if((start_of_chunk >= rats_tag_list.get_item(a).location) && (start_of_chunk < (rats_tag_list.get_item(a).location + rats_tag_list.get_item(a).allocation_size + 8)))
					{
						current_address = rats_tag_list.get_item(a).location + rats_tag_list.get_item(a).allocation_size + 8;
						has_found = false;
						start_of_chunk = current_address;
						found_chunk = 0;

						break;

					}

					// Please note: This method always works, since the found freespace always stops when it finds anything other than a 0x00. It won't overlap a RATS tag unless it's in the middle of it.
				}

			}

		// If the chunk is too small, restart looking:
		} else {
			found_chunk = 0;
			start_of_chunk = current_address + 1;
		}
		current_address++;

		if(current_address > rom_buffer.get_length())
		{
			qWarning() << "NO SPACE FOUND!" << current_address << rom_buffer.get_length();
			return 0xFFFFFFFF;
		}
	}

	pc_address found_pc_address = current_address - found_chunk;

	qWarning() << "FOUND FREESPACE AT" << found_pc_address.to_snes_format(has_header);
	return found_pc_address;
}

rats_tag SMW_ROM::allocate_rats(const rats_block &data)
{
	// Find freespace:
	rats_tag new_rats_tag;
	new_rats_tag.allocation_size = data.block.get_length();
	new_rats_tag.location = find_rats_freespace(data.block.get_length() + 8);

	if(new_rats_tag.location.is_valid() == false)
		return new_rats_tag;

	unsigned int current_index = new_rats_tag.location;

	// Insert the RATS tag:
	rom_buffer[current_index++] = 'S';
	rom_buffer[current_index++] = 'T';
	rom_buffer[current_index++] = 'A';
	rom_buffer[current_index++] = 'R';
	rom_buffer[current_index++] = (new_rats_tag.allocation_size - 1) & 0xFF;
	rom_buffer[current_index++] = ((new_rats_tag.allocation_size - 1) >> 8) & 0xFF;
	rom_buffer[current_index++] = ~((new_rats_tag.allocation_size - 1) & 0xFF);
	rom_buffer[current_index++] = ~(((new_rats_tag.allocation_size - 1) >> 8) & 0xFF);

	// Insert the RATS protected area data:
	rom_buffer.import_buffer_at(data.block, current_index);

	return new_rats_tag;
}

bool SMW_ROM::deallocate_rats(rats_tag tag)
{
	// If the RATS tag is below the expanded banks:
	if(tag.location.to_snes_format(has_header).bank() < 0x10)
		return false;

	// If this address contains a RATS tag:
	if(    rom_buffer[tag.location + 0] == 'S'
		&& rom_buffer[tag.location + 1] == 'T'
		&& rom_buffer[tag.location + 2] == 'A'
		&& rom_buffer[tag.location + 3] == 'R'
		&& (unsigned char)rom_buffer[tag.location + 4] == (unsigned char)~rom_buffer[tag.location + 6]
		&& (unsigned char)rom_buffer[tag.location + 5] == (unsigned char)~rom_buffer[tag.location + 7] )
	{
		// Calculate the size (nN + 1 + tag size):
		unsigned int size = (rom_buffer[tag.location + 4] | (rom_buffer[tag.location + 5] << 8)) + 8 + 1;

		// Clear out the area:
		for(unsigned int a=tag.location; a<(size + tag.location); a++)
		{
			rom_buffer[a] = 0x00;
		}

		return true;
	} else {
		// Invalid RATS tag passed, so return an error:
		return false;
	}
}
