#include "main.h"

bool level_object::set_type(unsigned char type)
{
	if((data_format == enum_object_data_format::hw_tt)
			|| (data_format == enum_object_data_format::hw_tt_ss)
			|| (data_format == enum_object_data_format::hw_tt_ss_ss))
	{
		settings_byte_2 = type;
		return true;
	}

	if(data_format == enum_object_data_format::tt)
	{
		settings_byte_1 = type;
		return true;
	}

	if(data_format == enum_object_data_format::ht)
	{
		unsigned char temp_byte = settings_byte_1 & 0xF0;

		settings_byte_1 = temp_byte | (type & 0x0F);
		return true;
	}

	if(data_format == enum_object_data_format::tw)
	{
		unsigned char temp_byte = settings_byte_1 & 0x0F;

		settings_byte_1 = temp_byte | ((type & 0x0F) << 4);
		return true;
	}

	// If none of the above, then this object does not have settings bytes giving the subtype. Return an error.
	return false;
}

bool level_object::set_width(int width)
{
	if((data_format == enum_object_data_format::uw)
			|| (data_format == enum_object_data_format::tw)
			|| (data_format == enum_object_data_format::hw)
			|| (data_format == enum_object_data_format::hw_tt)
			|| (data_format == enum_object_data_format::hw_tt_ss)
			|| (data_format == enum_object_data_format::hw_tt_ss_ss))
	{
		unsigned char temp_byte = settings_byte_1 & 0xF0;

		if(width > 0x0F)
			settings_byte_1 = temp_byte | ((0x0F));
		else if(width < 0x00)
			settings_byte_1 = temp_byte;
		else
			settings_byte_1 = temp_byte | ((width & 0x0F));

		return true;
	}

	if(data_format == enum_object_data_format::ww)
	{
		if(width > 0xFF)
			settings_byte_1 = 0xFF;
		else if(width < 0x00)
			settings_byte_1 = 0x00;
		else
			settings_byte_1 = width;

		return true;
	}

	// If none of the above, then this object does not have settings bytes. Return an error.
	return false;
}

bool level_object::set_height(int height)
{
	if((data_format == enum_object_data_format::hu)
			|| (data_format == enum_object_data_format::ht)
			|| (data_format == enum_object_data_format::hw)
			|| (data_format == enum_object_data_format::hw_tt)
			|| (data_format == enum_object_data_format::hw_tt_ss)
			|| (data_format == enum_object_data_format::hw_tt_ss_ss))
	{
		unsigned char temp_byte = settings_byte_1 & 0x0F;

		if(height > 0x0F)
			settings_byte_1 = temp_byte | ((0x0F) << 4);
		else if(height < 0x00)
			settings_byte_1 = temp_byte;
		else
			settings_byte_1 = temp_byte | ((height & 0x0F) << 4);

		return true;
	}

	if(data_format == enum_object_data_format::hh)
	{
		if(height > 0xFF)
			settings_byte_1 = 0xFF;
		else if(height < 0x00)
			settings_byte_1 = 0x00;
		else
			settings_byte_1 = height;

		return true;
	}

	// If none of the above, then this object does not have settings bytes. Return an error.
	return false;
}

unsigned char level_object::get_type()
{
	if((data_format == enum_object_data_format::hw_tt)
			|| (data_format == enum_object_data_format::hw_tt_ss)
			|| (data_format == enum_object_data_format::hw_tt_ss_ss))
	{
		return settings_byte_2;
	}

	if(data_format == enum_object_data_format::tt)
	{
		return settings_byte_1;
	}

	if(data_format == enum_object_data_format::ht)
	{
		unsigned char temp_byte = settings_byte_1 & 0x0F;

		return temp_byte;
	}

	if(data_format == enum_object_data_format::tw)
	{
		unsigned char temp_byte = (settings_byte_1 & 0xF0) >> 4;

		return temp_byte;
	}

	// If none of the above, then this object does not have settings bytes. Return an error.
	return 0;
}

unsigned int level_object::get_width()
{
	if((data_format == enum_object_data_format::uw)
			|| (data_format == enum_object_data_format::tw)
			|| (data_format == enum_object_data_format::hw)
			|| (data_format == enum_object_data_format::hw_tt)
			|| (data_format == enum_object_data_format::hw_tt_ss)
			|| (data_format == enum_object_data_format::hw_tt_ss_ss))
	{
		unsigned char temp_byte = settings_byte_1 & 0x0F;

		return temp_byte;
	}

	if(data_format == enum_object_data_format::ww)
	{
		return settings_byte_1;
	}

	// If none of the above, then this object does not have settings bytes. Return an error.
	return 0;
}

unsigned int level_object::get_height()
{
	if((data_format == enum_object_data_format::hu)
			|| (data_format == enum_object_data_format::ht)
			|| (data_format == enum_object_data_format::hw)
			|| (data_format == enum_object_data_format::hw_tt)
			|| (data_format == enum_object_data_format::hw_tt_ss)
			|| (data_format == enum_object_data_format::hw_tt_ss_ss))
	{
		unsigned char temp_byte = (settings_byte_1 & 0xF0) >> 4;

		return temp_byte;
	}

	if(data_format == enum_object_data_format::hh)
	{
		return settings_byte_1;
	}

	// If none of the above, then this object does not have settings bytes. Return an error.
	return 0;
}
