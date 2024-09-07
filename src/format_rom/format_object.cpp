#include "main.h"

object_block_queue::object_block_queue()
{
	buffer_low = new block_queue_item[16];
	buffer_high = new block_queue_item[16];
	true_length_low = 16;
	used_length_low = 0;
	true_length_high = 16;
	used_length_high = 0;
}

object_block_queue::~object_block_queue()
{
	delete[] buffer_low;
	delete[] buffer_high;
}

//block_queue_item * object_block_queue::getItem(unsigned int index)
//{
//	if(index >= used_length)
//		return NULL;
//
//	return &buffer[index];
//}

void object_block_queue::addItemLow(unsigned char block_low, unsigned int x_position, unsigned int y_position, unsigned int owner)
{
	if (true_length_low == used_length_low)
	{
		// Store pointer to old buffer, so it can be copied and deleted soon:
		block_queue_item * temp_buffer = buffer_low;

		// Create larger buffer:
		buffer_low = new block_queue_item[true_length_low + 16];

		// Copy buffers:
		int a;
		for (a=0;a<true_length_low;a++) {
			buffer_low[a].block_byte = temp_buffer[a].block_byte;
			buffer_low[a].x_position = temp_buffer[a].x_position;
			buffer_low[a].y_position = temp_buffer[a].y_position;
			buffer_low[a].owner = temp_buffer[a].owner;
		}

		// Update true_length:
		true_length_low += 16;

		// Delete old buffer:
		delete[] temp_buffer;
	}

	// Add item:
	buffer_low[used_length_low].block_byte = block_low;
	buffer_low[used_length_low].x_position = x_position;
	buffer_low[used_length_low].y_position = y_position;
	buffer_low[used_length_low].owner = owner;

	// Increase used_length:
	used_length_low++;
}

void object_block_queue::addItemHigh(unsigned char block_high, unsigned int x_position, unsigned int y_position, unsigned int owner)
{
	if (true_length_high == used_length_high)
	{
		// Store pointer to old buffer, so it can be copied and deleted soon:
		block_queue_item * temp_buffer = buffer_high;

		// Create larger buffer:
		buffer_high = new block_queue_item[true_length_high + 16];

		// Copy buffers:
		int a;
		for (a=0;a<true_length_high;a++) {
			buffer_high[a].block_byte = temp_buffer[a].block_byte;
			buffer_high[a].x_position = temp_buffer[a].x_position;
			buffer_high[a].y_position = temp_buffer[a].y_position;
			buffer_high[a].owner = temp_buffer[a].owner;
		}

		// Update true_length:
		true_length_high += 16;

		// Delete old buffer:
		delete[] temp_buffer;
	}

	// Add item:
	buffer_high[used_length_high].block_byte = block_high;
	buffer_high[used_length_high].x_position = x_position;
	buffer_high[used_length_high].y_position = y_position;
	buffer_high[used_length_high].owner = owner;

	// Increase used_length:
	used_length_high++;
}

void object_block_queue::applyQueue(level_map_data & level_map)
{
	// TODO: add integrity check
	// TODO: add objectool bugtracker

	unsigned int a;

	// Set high byte:
	for(a=0;a<used_length_high;a++)
	{
		level_map.setMapDataHigh(buffer_high[a].x_position, buffer_high[a].y_position, buffer_high[a].block_byte, buffer_high[a].owner);
	}

	// Set low byte:
	for(a=0;a<used_length_low;a++)
	{
		level_map.setMapDataLow(buffer_low[a].x_position, buffer_low[a].y_position, buffer_low[a].block_byte, buffer_low[a].owner);
	}
}

void object_block_queue::get_size_and_position(int & size_x, int & size_y, int & position_x, int & position_y)
{
	unsigned int a;
	int smallest_x = -1;
	int smallest_y = -1;
	int largest_x = 1024;
	int largest_y = 1024;

	if(used_length_low == 0)
	{
		size_x = 0;
		size_y = 0;
		position_x = 0;
		position_y = 0;
		return;
	}

	// Only check the low byte for speed:
	for(a=0;a<used_length_low;a++)
	{
		if(smallest_x > buffer_low[a].x_position)
			smallest_x = buffer_low[a].x_position;

		if(smallest_y > buffer_low[a].y_position)
			smallest_y = buffer_low[a].y_position;

		if(largest_x < buffer_low[a].x_position)
			largest_x = buffer_low[a].x_position;

		if(largest_y < buffer_low[a].y_position)
			largest_y = buffer_low[a].y_position;
	}

	size_x = largest_x - smallest_x;
	size_y = largest_y - smallest_y;
	position_x = smallest_x;
	position_y = smallest_y;
}
