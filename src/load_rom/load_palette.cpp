#include "main.h"

emulation_error load_vanilla_palette(const SMW_ROM &rom, GLuint palette_texture, level_header header)
{
	unsigned char palette[256 * 3];

	object_emulator emulator(rom, 0, palette_texture);

	// Set emulator settings:
	emulator.upload_to_ram(header.FG_palette, 0x7E192D);		// Foreground palette settings.
	emulator.upload_to_ram(header.sprite_palette, 0x7E192E);	// Sprite palette settings.
	emulator.upload_to_ram(header.BG_color, 0x7E192F);			// Background color settings.
	emulator.upload_to_ram(header.BG_palette, 0x7E1930);		// Background palette settings.

	emulation_error error = emulator.emulate(0x00ABED, 0x00);	// TODO: Error report!

	// Decode palette data into an OpenGL-readable format:
	for(int i=0; i<256; i++)
	{
		unsigned char byte_2 = emulator.download_from_ram(0x7E0703 + i * 2);
		unsigned char byte_1 = emulator.download_from_ram(0x7E0703 + i * 2 + 1);

		unsigned char blue = ((byte_1 & 0x7C) >> 2) << 3;
		unsigned char green = (((byte_1 & 0x03) << 3) | ((byte_2 & 0xE0) >> 5)) << 3;
		unsigned char red = ((byte_2 & 0x1F)) << 3;

		palette[i*3 + 0] = red;
		palette[i*3 + 1] = green;
		palette[i*3 + 2] = blue;
	}

	// Upload to OpenGL:
	glBindTexture(GL_TEXTURE_2D, palette_texture);

	glTexSubImage2D(GL_TEXTURE_2D,
		0,
		0,
		0,
		16,
		16,
		GL_RGB,
		GL_UNSIGNED_BYTE,
		reinterpret_cast<void*>(palette));

	return error;
}
