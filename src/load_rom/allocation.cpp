#include "allocation.h"
#include "load_rom.h"

#include <QDebug>

rats_block::rats_block()
{
	block.reset_size(4);
	block[0] = 'R';
	block[1] = 'P';
	block[2] = 'N';
	block[3] = 'S';
}

rats_block::rats_block(const rats_tag &tag, const SMW_ROM &rom)
{
	block.reset_size(tag.allocation_size);

	pc_address corrected_address = tag.location + 8;	// Remove the RATS tag from the data block.

	for(int a=0; a<tag.allocation_size; a++)
	{
		block.set_byte(a, rom.get_byte(corrected_address.to_snes_format(rom.has_header)));
		corrected_address++;
	}

	if(		((block[0] == 'R') &&
			(block[1] == 'P') &&
			(block[2] == 'N') &&
			(block[3] == 'S')) == false)
	{
		is_rpns_format = false;
		return;
	}

	int a = 4;	// Start past the RPNS header.
	while(a<block.get_length())
	{
		if(block[a + 0] == 'X')
		{
			length++;
			a += (block[a + 2] | (block[a + 3] << 8)) + 1 + 4;
		} else {
			is_rpns_format = false;
			return;
		}
	}
}

unsigned int rats_block::append_partition(const buffer & data, partition_type type)
{
	if(data.get_length() == 0)
		return 0;

	unsigned int current_index = block.get_length();

	buffer old_block = block;								// Copy the old data before reassigning its contents.
	block.reset_size(old_block.get_length() + data.get_length() + 4);	// Allocate a new buffer with the size of the old buffer + the new data + the new tag header.

	block.import_buffer_at(old_block, 0x00);					// Copy the old data back in.

	// Header definition:
	block[current_index++] = 'X';
	block[current_index++] = (unsigned char)type;
	block[current_index++] = (data.get_length() - 1) & 0xFF;
	block[current_index++] = ((data.get_length() - 1) >> 8) & 0xFF;

	// Copy the buffer:
	block.import_buffer_at(data, current_index);

	length++;

	return current_index;
}

partition_type rats_block::get_partition_type(unsigned int index)
{
	if(index >= length)
		return partition_type::invalid;

	unsigned int current_index = 0;

	int a = 4;	// Start past the RPNS header.
	while(a<block.get_length())
	{
		if(	   block[a + 0] == 'X'
			   )
		{
			if(index == current_index)
				return (partition_type)(unsigned char)block[a + 1];

			current_index++;
			a += (block[a + 2] | (block[a + 3] << 8)) + 1 + 4;
		} else {
			return partition_type::invalid;
		}
	}
	return partition_type::invalid;
}

buffer rats_block::get_partition(unsigned int index)
{
	if(index >= length)
		return buffer();


}
