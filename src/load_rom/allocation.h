#ifndef ALLOCATION_H
#define ALLOCATION_H

#include "utilities/address.h"
#include "utilities/buffer.h"

#define partition_type_tostring(index)	partition_type_string::string[(int)index]

namespace partition_type_string {
  static const char* string[] = {
	  "Invalid",
	  "Name",
	  "Level Layer 1",
	  "Level Layer 2",
	  "Level Sprites"
  };
}

class SMW_ROM;

enum struct partition_type
{
	invalid = 0,
	name = 1,
	level_layer_1 = 2,
	level_layer_2 = 3,
	level_sprites = 4
};

struct rats_tag
{
	pc_address location;			// The location of the start of the RATS tag.
	unsigned int allocation_size;	// The size of the allocated chunk (not counting the tag).
};

class rats_block
{
public:
	rats_block();
	rats_block(const rats_tag &tag, const SMW_ROM &rom);

	unsigned int append_partition(const buffer &data, partition_type type);
	unsigned int get_length() const
	{
		return length;
	}

	partition_type get_partition_type(unsigned int index);
	buffer get_partition(unsigned int index);

	buffer block;

	bool is_rpns_format = true;
private:
	unsigned int length = 0;
};


#endif // ALLOCATION_H
