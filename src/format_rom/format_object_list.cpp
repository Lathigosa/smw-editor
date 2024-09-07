#include "main.h"

namespace static_string {
  static const char* standard_object_names[] = {
	  "Extended Objects",
	  "Blue Water",
	  "Invisible Coin Blocks",
	  "Invisible Note Blocks",
	  "Invisible POW Coins",
	  "Coins",
	  "Walk-Through Dirt",
	  "Colored Water",
	  "Note Blocks",
	  "Turn Blocks",
	  "Question Blocks (Coins)",
	  "Throw Blocks",
	  "Munchers",
	  "Cement Blocks",
	  "Used Blocks",
	  "Vertical Pipes",
	  "Horizontal Pipes",
	  "Bullet Shooter",
	  "Terrain Slopes",
	  "Terrain Ledge Edges",
	  "Terrain Ledge",
	  "Goal Poles",
	  "Blue Coins",
	  "Rope/Clouds",
	  "Water with Animated Surface",
	  "Water with Still Surface",
	  "Lava with Animated Surface",
	  "Net Top Edge",
	  "Donut Bridge",
	  "Net Bottom Edge",
	  "Net Vertical Edge",
	  "Vertical Log",
	  "Horizontal Log",
	  "Terrain Long Ledge"
  };

  static const char* tileset_object_names[][18] =
  {
	  {
		"Unused",
		"Unused",
		"Ice Blue Vertical Pipe",
		"Ice Blue Turn Tiles",
		"Blue Switch Blocks",
		"Forest Canopy",
		"Forest Ledge Edges",
		"Forest Ledge",
		"Forest Tree Trunk - Large",
		"Forest Tree Trunk - Small",
		"Red Switch Blocks",
		"Diagonal Pipe",
		"Diagonal Ledge - Left",
		"Diagonal Ledge - Right",
		"Arch Ledge",
		"Top Cloud Fringe",
		"Left/Right Cloud Fringe",
		"Bushes"
	  },
	  {
		"Unused",
		"Unused",
		"Unused",
		"Unused",
		"Unused",
		"Unused",
		"Double Ended Red-Green Pipe",
		"Rock Wall Background",
		"Large Spikes",
		"Horizontal Guide Lines",
		"Vertical Guide Lines",
		"Blue Switch Blocks",
		"Red Switch Blocks",
		"Ledge",
		"Stone Block Wall",
		"Conveyors",
		"Horizontal Row of Spikes",
		"Vertical Column of Spikes"
	  },
	  {
		"Unused",
		"Unused",
		"Unused",
		"Unused",
		"Log Bridge",
		"Blue Switch Blocks",
		"Red Switch Blocks",
		"Plants on Columns",
		"Horizontal Conveyors",
		"Diagonal Conveyors",
		"Horizontal Guide Lines",
		"Vertical Guide Lines",
		"Miscellaneous Guide Lines",
		"Very Steep Guide Lines",
		"Mushroom Ledge",
		"Mushroom Column",
		"Horizontal Log",
		"Vertical Log"
	  },
	  {
		"Unused",
		"Unused",
		"Unused",
		"Unused",
		"Unused",
		"Unused",
		"Blue Switch Blocks",
		"Red Switch Blocks",
		"Terrain Four-Sided Edge",
		"Multiple Canvasses (Hardcoded Position)",
		"Right Facing Mud/Lava",
		"Mud/Lava Slopes",
		"Mud/Lava with Top",
		"Mud/Lava Tiles",
		"Very Steep Slopes",
		"Upside-Down Ledge",
		"Solid Edges",
		"Solid Dirt"
	  },
	  {
		  "Horizontal Upward Spikes",
		  "Log Background",
		  "Grass Ledge 1",
		  "Wooden Crate",
		  "Grass Ledge 2",
		  "Clouds",
		  "Wooden Ledge on Column",
		  "Grey Bricks Background",
		  "Wooden Blocks",
		  "Horizontal Log Background",
		  "Wooden Ledge",
		  "Vertical Log Backgrounds",
		  "Miscellaneous Edges",
		  "Stone Ledge",
		  "Switch Palace Upside Down Ledge",
		  "Switch Palace Ledge",
		  "Switch Palace Right Facing Edge",
		  "Switch Palace Left Facing Edge"
	  }
  };
}

object_toolbox::object_toolbox()
{
}

rom_error object_toolbox::load_items_format(const SMW_ROM & rom, enum_tileset current_tileset)
{
	/*
	 * Please note:
	 * The addresses 0x01FFD0-0x01FFFF are normally empty [0xFF] in a clean rom. This
	 * section in the ROM gets used by the SMW Editor to store the pointers to the
	 * object format tables for each tileset. These tables, which this table points
	 * to, contain 4 bits per object for each tileset. These four bits represent the
	 * way their data bits should be interpreted. See "enum_object_data_format".
	*/

	// Load the table which tells the address of the object format tables for each tileset:
	unsigned char pointer_low  = rom.get_byte_pc(PC_ADDRESS(0x01FFD0 + 3*((int)current_tileset), rom.has_header));
	unsigned char pointer_high = rom.get_byte_pc(PC_ADDRESS(0x01FFD1 + 3*((int)current_tileset), rom.has_header));
	unsigned char pointer_bank = rom.get_byte_pc(PC_ADDRESS(0x01FFD2 + 3*((int)current_tileset), rom.has_header));
	unsigned int pointer_to_table = pointer_low | (pointer_high << 8) | (pointer_bank << 16);

	rom_error current_error = rom_error::success;

	// If the pointer is empty, then it is a clean rom or one edited by Lunar Magic. Store the vanilla pointers in this spot:
	if(pointer_to_table == 0xFFFFFF)
	{
		// TODO: recognize whether a ROM was edited by Lunar Magic.

		// If the ROM is clean, load the vanilla item format list:
		current_error = load_vanilla_items_format(current_tileset);
	}

	// Test for any objects that are longer than three bytes (Lunar Magic, for example):
	object_emulator emulator(rom, 0, 0);

	level_header header;
	header.tile_set = (int)current_tileset;

	// Start at 1, since 0 contains extended objects and is an exception:
	for(int a = 1; a<0x40; a++)
	{
		level_object object;
		object.object_number = a;
		object.new_screen = 0;
		object.screen_number = 0;
		object.position_X = 0;
		object.position_Y = 0;
		object.settings_byte_1 = 0x00;

		// Reset the level data address pointer in the emulator's ram to 0x0000, so we can easily see when it's incremented:
		emulator.upload_to_ram(0x00, 0x7E0065);
		emulator.upload_to_ram(0x00, 0x7E0066);

		object.decode_object(rom, emulator, header, 1);

		// Check the level data address pointer in the emulator's ram to see if it has incremented. If it has, then the object is longer than three bytes.
		if(emulator.download_from_ram(0x7E0065) == 0x00)
		{

		} else if (emulator.download_from_ram(0x7E0065) == 0x01)
		{
			items[a].format = enum_object_data_format::hw_tt;
		} else if (emulator.download_from_ram(0x7E0065) == 0x02)
		{
			items[a].format = enum_object_data_format::hw_tt_ss;
		} else if (emulator.download_from_ram(0x7E0065) == 0x03)
		{
			items[a].format = enum_object_data_format::hw_tt_ss_ss;
		} else
		{
			qWarning() << "Error, unsupported byte length!" << a << ":" << emulator.download_from_ram(0x7E0065);
			// TODO: Error, so many bytes are not supported.
		}
	}





	// This is a normal object, grab routine pointer from second table in ROM: 0x0DA455 0x0DA456 0x0DA457
	//unsigned char address_low  = rom.getByte(PC_ADDRESS(pointer_to_table + 10 + 3*(object_list.getObject(a)->object_number - 1), rom.has_header));
	//unsigned char address_high = rom.getByte(PC_ADDRESS(pointer_to_table + 11 + 3*(object_list.getObject(a)->object_number - 1), rom.has_header));
	//unsigned char address_bank = rom.getByte(PC_ADDRESS(pointer_to_table + 12 + 3*(object_list.getObject(a)->object_number - 1), rom.has_header));
	//unsigned int address = (address_low) | (address_high << 8) | (address_bank << 16);



	//int a;
	//for(a=0; a<=0x3F; a++)
	//{
	//	items[a].format = enum_object_data_format::hh;
	//}

	return current_error;
}

rom_error object_toolbox::load_vanilla_items_format(enum_tileset current_tileset)
{
	// Initialize all objects to have the format type "height-width":
	for(int a=0; a<=0x3F; a++)
	{
		items[a].format = enum_object_data_format::hw;
		items[a].number_of_subtypes = 0;
	}

	// Change some of the objects to their true values:
	items[0x00].format = enum_object_data_format::tt;			// 0x00: Extended Objects
	items[0x00].number_of_subtypes = 0x98;
	items[0x0F].format = enum_object_data_format::ht;			// 0x0F: Vertical Pipes
	items[0x0F].number_of_subtypes = 6;
	items[0x10].format = enum_object_data_format::tw;			// 0x10: Horizontal Pipes
	items[0x10].number_of_subtypes = 4;
	items[0x11].format = enum_object_data_format::hu;			// 0x11: Canon
	items[0x12].format = enum_object_data_format::ht;			// 0x12: Slopes
	items[0x12].number_of_subtypes = 16;
	items[0x13].format = enum_object_data_format::ht;			// 0x13: Ledge Edges
	items[0x13].number_of_subtypes = 15;
	items[0x15].format = enum_object_data_format::ht;			// 0x15: Midway/Goal Point
	items[0x15].number_of_subtypes = 2;
	items[0x17].format = enum_object_data_format::tw;			// 0x17: Rope/Clouds
	items[0x17].number_of_subtypes = 2;
	items[0x1C].format = enum_object_data_format::uw;			// 0x1C: Donut Bridge
	items[0x1E].format = enum_object_data_format::ht;			// 0x1E: Net Vertical Edge
	items[0x1E].number_of_subtypes = 2;
	items[0x1F].format = enum_object_data_format::hu;			// 0x1F: Vertical Pipe/Bone/Log
	items[0x20].format = enum_object_data_format::uw;			// 0x20: Horizontal Pipe/Bone/Log
	items[0x21].format = enum_object_data_format::ww;			// 0x21: Long Ground Ledge

	// Set the names:
	for(int a=0x00; a<=0x21; a++)
	{
		items[a].name = static_string::standard_object_names[a];
	}

	for(int a=0x22; a<=0x2D; a++)
	{
		items[a].name = "Unused";
	}

	int tileset_type = 0;
	if(current_tileset == enum_tileset::tileset_normal_1 || current_tileset == enum_tileset::tileset_normal_2 || current_tileset == enum_tileset::tileset_cloud_forest)
		tileset_type = 0;
	else if(current_tileset == enum_tileset::tileset_castle_1)
		tileset_type = 1;
	else if(current_tileset == enum_tileset::tileset_rope_1 || current_tileset == enum_tileset::tileset_rope_2 || current_tileset == enum_tileset::tileset_rope_3)
		tileset_type = 2;
	else if(current_tileset == enum_tileset::tileset_underground_1 || current_tileset == enum_tileset::tileset_underground_2 || current_tileset == enum_tileset::tileset_switch_palace_2 || current_tileset == enum_tileset::tileset_castle_2 || current_tileset == enum_tileset::tileset_underground_3)
		tileset_type = 3;
	else if(current_tileset == enum_tileset::tileset_switch_palace_1 || current_tileset == enum_tileset::tileset_ghost_house_1 || current_tileset == enum_tileset::tileset_ghost_house_2)
		tileset_type = 4;

	for(int a=0x2E; a<=0x3F; a++)
	{
		items[a].name = static_string::tileset_object_names[(int)tileset_type][a - 0x2E];
	}

	// Change the tileset specific objects' data:
	if(tileset_type == 0)
	{
		items[0x30].format = enum_object_data_format::hu;
		items[0x33].format = enum_object_data_format::ww;
		items[0x34].format = enum_object_data_format::ht;
		items[0x34].number_of_subtypes = 4;
		items[0x36].format = enum_object_data_format::hu;
		items[0x37].format = enum_object_data_format::hu;
		items[0x39].format = enum_object_data_format::hu;
		items[0x3C].format = enum_object_data_format::uw;
		items[0x3D].format = enum_object_data_format::tw;
		items[0x3D].number_of_subtypes = 2;
		items[0x3E].format = enum_object_data_format::ht;
		items[0x3E].number_of_subtypes = 8;
		items[0x3F].format = enum_object_data_format::tw;
		items[0x3F].number_of_subtypes = 3;
	} else if(tileset_type == 1)
	{
		items[0x34].format = enum_object_data_format::hu;
		items[0x36].format = enum_object_data_format::hu;
		items[0x37].format = enum_object_data_format::tw;
		items[0x37].number_of_subtypes = 2;
		items[0x38].format = enum_object_data_format::ht;
		items[0x38].number_of_subtypes = 2;
		items[0x3B].format = enum_object_data_format::uw;
		items[0x3D].format = enum_object_data_format::ht;
		items[0x3D].number_of_subtypes = 4;
		items[0x3E].format = enum_object_data_format::tw;
		items[0x3E].number_of_subtypes = 2;
		items[0x3F].format = enum_object_data_format::ht;
		items[0x3F].number_of_subtypes = 2;
	} else if(tileset_type == 2)
	{
		items[0x32].format = enum_object_data_format::uw;
		items[0x35].format = enum_object_data_format::ht;
		items[0x35].number_of_subtypes = 4;
		items[0x36].format = enum_object_data_format::tw;
		items[0x36].number_of_subtypes = 2;
		items[0x37].format = enum_object_data_format::ht;
		items[0x37].number_of_subtypes = 4;
		items[0x38].format = enum_object_data_format::tw;
		items[0x38].number_of_subtypes = 2;
		items[0x39].format = enum_object_data_format::ht;
		items[0x39].number_of_subtypes = 3;
		items[0x3A].format = enum_object_data_format::ht;
		items[0x3A].number_of_subtypes = 6;
		items[0x3B].format = enum_object_data_format::tw;
		items[0x3B].number_of_subtypes = 2;
		items[0x3C].format = enum_object_data_format::uw;
		items[0x3E].format = enum_object_data_format::uw;
		items[0x3F].format = enum_object_data_format::hu;
	} else if(tileset_type == 3)
	{
		items[0x37].format = enum_object_data_format::uw;
		items[0x38].format = enum_object_data_format::ht;
		items[0x38].number_of_subtypes = 2;
		items[0x39].format = enum_object_data_format::ht;
		items[0x39].number_of_subtypes = 4;
		items[0x3C].format = enum_object_data_format::tw;
		items[0x3C].number_of_subtypes = 2;
		items[0x3E].format = enum_object_data_format::ht;
		items[0x3E].number_of_subtypes = 4;
	} else if(tileset_type == 4)
	{
		items[0x2E].format = enum_object_data_format::uw;
		items[0x33].format = enum_object_data_format::uw;
		items[0x34].format = enum_object_data_format::hw;
		items[0x37].format = enum_object_data_format::tw;
		items[0x37].number_of_subtypes = 3;
		items[0x38].format = enum_object_data_format::uw;
		items[0x39].format = enum_object_data_format::ht;
		items[0x39].number_of_subtypes = 3;
		items[0x3A].format = enum_object_data_format::ht;
		items[0x3A].number_of_subtypes = 4;
		items[0x3B].format = enum_object_data_format::uw;
	}

	return rom_error::success;
}
