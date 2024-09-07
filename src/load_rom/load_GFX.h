#ifndef LOAD_GFX_H
#define LOAD_GFX_H

//#include "main.h"

#include "load_rom/load_rom.h"
#include "format_rom/format_level.h"
#include "utilities/buffer.h"
#include <QtOpenGL>

enum error_gfx {
	success = 0,				// The GFX file was successfully uploaded.
	tile_mismatch = 1,			// The amount of tiles is not a multiple of 16.
	pixel_mismatch = 2,			// The amount of pixels do not fully form tiles.
	texture_too_small = 3,		// The texture given does not fit the full GFX file, indicating the GFX file is too large or the tile offset is wrong.
	texture_size_mismatch = 4,	// The texture is not a size in increments of 8.
	illegal_argument = 5		// One of the passed arguments is wrong.
};

// The bits per pixel.
enum BPP {
	BPP_2 = 2,
	BPP_3 = 3,
	BPP_4 = 4,
	BPP_8 = 8
};

// Converts a BPP_3 gfx buffer to a BPP_4 one.
error_gfx convert_3bpp_to_4bpp(buffer &GFX_buffer, buffer &output_buffer);

// Loads a GFX file from ROM (GFX00-GFX31) into memory.
// Do not use this over load_gfx_to_texture unless necessary.
//void load_gfx(unsigned int page_number, char * buffer);

// Loads a GFX file from ROM into a texture. Requires the texture to be the right size.
error_gfx load_gfx_page(const SMW_ROM & rom, GLuint texture, unsigned char gfx_page);

// Loads a GFX file from a buffer into an OpenGL texture.
// Use this if you want to show the GFX file on-screen or
//   want to manipulate it using shaders.
// For normal GFX files (GFX00-GFX31), the v_tile_offset should be 8 * page_in_texture.
// If it returns anything other than 0, the texture hasn't been uploaded and the texture is left intact.
// @ buffer_offset is the byte padding at the start of GFX_buffer before the actual GFX file begins (for example, in raw emulator RAM).
// @ buffer_length is the amount of bytes to decode, or 0 if the entire buffer should be decoded.
// @ bits_per_pixel can be 2BPP, 3BPP, 4BPP or 8BPP.
// @ pixel_size is the size of the texture
// @ tile_offset is the size of the tiles to be placed.
error_gfx load_gfx_to_texture(buffer &GFX_buffer, unsigned int buffer_offset, unsigned int buffer_length, BPP bits_per_pixel, GLuint texture, unsigned int x_tile_size, unsigned int y_tile_size, unsigned int x_tile_offset, unsigned int y_tile_offset);

void load_vanilla_gfx(const SMW_ROM &rom, GLuint terrain_texture, GLuint sprite_texture, level_header header);

#endif // LOAD_GFX_H
