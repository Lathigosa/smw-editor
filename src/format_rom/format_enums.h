#ifndef FORMAT_ENUMS_H
#define FORMAT_ENUMS_H

//#include "main.h"

enum struct enum_tileset {
	tileset_normal_1		= 0,
	tileset_castle_1		= 1,
	tileset_rope_1			= 2,
	tileset_underground_1	= 3,
	tileset_switch_palace_1	= 4,
	tileset_ghost_house_1	= 5,
	tileset_rope_2			= 6,
	tileset_normal_2		= 7,
	tileset_rope_3			= 8,
	tileset_underground_2	= 9,
	tileset_switch_palace_2	= 10,
	tileset_castle_2		= 11,
	tileset_cloud_forest	= 12,
	tileset_ghost_house_2	= 13,
	tileset_underground_3	= 14,
	tileset_illegal			= 15
};

enum struct enum_object_data_format
{
	hh = 0,
	hw = 1,
	error_1 = 2,
	ww = 3,
	error_2 = 4,
	tw = 5,
	error_3 = 6,
	tt = 7,
	ht = 8,
	hu = 9,
	uw = 10,
	hw_tt = 11,
	error_4 = 12,
	hw_tt_ss = 13,
	hw_tt_ss_ss = 14,
	unused = 15,
};

namespace object_data_format_string {
  static const char* string[] = {
	  "hh",
	  "hw",
	  "--",
	  "ww",
	  "--",
	  "tw",
	  "--",
	  "tt",
	  "ht",
	  "hu",
	  "uw",
	  "hw_tt",
	  "--",
	  "hw_tt_ss",
	  "hw_tt_ss_ss",
	  "--"
  };

  static const char* string_long[] = {
	  "Long Height",
	  "Height, Width",
	  "Error!",
	  "Long Width",
	  "Error!",
	  "Type, Width",
	  "Error!",
	  "Long Type",
	  "Height, Type",
	  "Height",
	  "Width",
	  "Height, Width, Long Type",
	  "Error!",
	  "Height, Width, Long Type, Long Settings",
	  "Height, Width, Long Type, Long Settings, Long Settings",
	  "Error!"
  };
}

#endif // FORMAT_ENUMS_H
