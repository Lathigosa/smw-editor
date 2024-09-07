#ifndef FORMAT_OBJECT_H
#define FORMAT_OBJECT_H

//#include "main.h"

#include "format_level.h"

struct block_queue_item
{
	unsigned char block_byte;

	unsigned int x_position;
	unsigned int y_position;

	unsigned int owner;			// The object index this block belongs to.
};

class object_block_queue
{
public:
	object_block_queue();
	~object_block_queue();

	//block_queue_item * getItemLow(unsigned int index);
	void addItemLow(unsigned char block_low, unsigned int x_position, unsigned int y_position, unsigned int owner);
	void addItemHigh(unsigned char block_high, unsigned int x_position, unsigned int y_position, unsigned int owner);

	void applyQueue(level_map_data & level_map);

	void get_size_and_position(int & size_x, int & size_y, int & position_x, int & position_y);

private:
	block_queue_item * buffer_low;
	block_queue_item * buffer_high;

	unsigned int true_length_low;
	unsigned int used_length_low;

	unsigned int true_length_high;
	unsigned int used_length_high;
};

#endif // FORMAT_OBJECT_H
