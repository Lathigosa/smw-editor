#ifndef LOAD_MAP16_H
#define LOAD_MAP16_H

#include "main.h"

// Loads the vanilla map16 maps based on the tileset specified.
bool load_vanilla_map16(map16_map &output_buffer, const SMW_ROM &rom, unsigned char tileset);

#endif // LOAD_MAP16_H
