#include "main.h"

buffer encode_level(queue<level_object> & object_list, level_header & header)
{
	// The buffer length is initially 5 (header) + 1 (end 0xFF byte) = 6:
	int buffer_length = 6;

	// Calculate the size of the level:
	for(int a=0; a<object_list.get_length(); a++)
	{
		if(object_list.get_item(a).data_format == enum_object_data_format::hw_tt)
			buffer_length+=4;
		else if(object_list.get_item(a).data_format == enum_object_data_format::hw_tt_ss)
			buffer_length+=5;
		else if(object_list.get_item(a).data_format == enum_object_data_format::hw_tt_ss_ss)
			buffer_length+=6;
		else
			buffer_length+=3;
	}

	// Update the new_screen flags:
	for(int a=0; a<object_list.get_length(); a++)
	{
		level_object temp_object = object_list.get_item(a);

		temp_object.new_screen = false;

		// If the screen number is incremented in comparison to the previous object, then the new_screen flag should be set:
		if(object_list.get_item(a).screen_number == object_list.get_item(a - 1).screen_number + 1)
			temp_object.new_screen = true;

		// If the screen numbers are too much different, add a screen jump object:
		else if(object_list.get_item(a).screen_number != object_list.get_item(a - 1).screen_number)
			buffer_length+=3;

		object_list.set_item(a, temp_object);
	}

	buffer output_buffer(buffer_length);

	// Set header:
	output_buffer.set_byte(0,
						  ((header.BG_palette & 0x07) << 5)
						| (header.amount_of_screens & 0x1F));
	output_buffer.set_byte(1,
						  ((header.BG_color & 0x07) << 5)
						| (header.level_mode & 0x1F));
	output_buffer.set_byte(2,
						  ((header.layer_3_priority & 0x01) << 7)
						| ((header.music & 0x07) << 4)
						| (header.sprite_set & 0x0F));
	output_buffer.set_byte(3,
						  ((header.time_limit & 0x03) << 6)
						| ((header.sprite_palette & 0x07) << 3)
						| (header.FG_palette & 0x07));
	output_buffer.set_byte(4,
						  ((header.item_memory & 0x03) << 6)
						| ((header.vertical_scroll & 0x03) << 4)
						| (header.tile_set & 0x0F));


	// === Start filling the buffer with actual object data: ===
	int index = 5;

	// All others:
	for(int a=0; a<object_list.get_length(); a++)
	{
		// If there should be a screen jump object, add it:
		if((object_list.get_item(a).screen_number != object_list.get_item(a - 1).screen_number)
				&& (object_list.get_item(a).new_screen == false))
		{
			output_buffer.set_byte(index, object_list.get_item(a).screen_number & 0x1F);
			index++;
			output_buffer.set_byte(index, 0x00);
			index++;
			output_buffer.set_byte(index, 0x01);
			index++;
		}

		unsigned int position_x;
		unsigned int position_y;

		// In vertical maps, the X and Y bits are swapped:
		if(header.is_vertical_level == false) {
			position_x = object_list.get_item(a).position_X;
			position_y = object_list.get_item(a).position_Y;
		} else {
			position_x = object_list.get_item(a).position_Y;
			position_y = object_list.get_item(a).position_X;
		}

		// Add this object:
		output_buffer.set_byte(index, (object_list.get_item(a).new_screen * 0x80)
								| ((object_list.get_item(a).object_number & 0x30) << 1)
								| (position_y & 0x1F));
		index++;
		output_buffer.set_byte(index, ((object_list.get_item(a).object_number & 0x0F) << 4)
								| (position_x & 0x0F));
		index++;
		output_buffer.set_byte(index, object_list.get_item(a).settings_byte_1);
		index++;

		// If the object byte size is larger:
		if(object_list.get_item(a).data_format == enum_object_data_format::hw_tt)
		{
			output_buffer.set_byte(index, object_list.get_item(a).settings_byte_2);
			index++;
		}
		else if(object_list.get_item(a).data_format == enum_object_data_format::hw_tt_ss)
		{
			output_buffer.set_byte(index, object_list.get_item(a).settings_byte_2);
			index++;
			output_buffer.set_byte(index, object_list.get_item(a).settings_byte_3);
			index++;
		}
		else if(object_list.get_item(a).data_format == enum_object_data_format::hw_tt_ss_ss)
		{
			output_buffer.set_byte(index, object_list.get_item(a).settings_byte_2);
			index++;
			output_buffer.set_byte(index, object_list.get_item(a).settings_byte_3);
			index++;
			//output_buffer.setByte(index, object_list.get_object(a)->settings_byte_4);
			index++;
		}

	}

	output_buffer.set_byte(index, 0xFF);

	return output_buffer;
}
