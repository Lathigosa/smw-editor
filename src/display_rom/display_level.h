#ifndef DISPLAY_LEVEL_H
#define DISPLAY_LEVEL_H

//#include "main.h"
#include "ui_rendering/utilities/level_rendering.h"
#include "format_rom/format_level.h"

// This function converts one map16 tile to 8x8 graphics
// tiles, by looking up the tiles in the map16 data. It puts
// the tiles in the level map texture. This should be called
// each time map16 is updated or if the level is altered.
void display_level_tile(const map16_map &map16, unsigned char map16_high, unsigned char map16_low, unsigned int x_position, unsigned int y_position, GLuint texture);

// This function converts the level map data to 8x8 graphics
// tiles, by looking up the tiles in the map16 data. It puts
// the tiles in the level map texture. This should be called
// each time map16 is updated or if the level is altered. It
// displays the entire level_map_data map.
emulation_error display_level_map(const map16_map &map16, level_map_data &level_map, GLuint texture);

// This function converts the level map data to 8x8 graphics
// tiles, by looking up the tiles in the map16 data. It puts
// the tiles in the level map texture. This should be called
// each time map16 is updated or if the level is altered. It
// displays only a section of the level_map_data map.
emulation_error display_level_map(const map16_map &map16, level_map_data &level_map, GLuint texture, unsigned int x_min, unsigned int y_min, unsigned int x_max, unsigned int y_max);

emulation_error prepare_display_level_sprites(sprite_toolbox &toolbox);

emulation_error display_level_sprites(const queue<level_sprite> &sprite_list, queue<opengl_sprite> &render_list, const sprite_toolbox &toolbox);

#endif // DISPLAY_LEVEL_H
