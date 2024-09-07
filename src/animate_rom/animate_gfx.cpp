#include "main.h"

void upload_vanilla_gfx_animation(object_emulator & animation_emulator, const SMW_ROM & rom)
{
	buffer GFX_32_buffer;
	buffer GFX_33_buffer;

	decompress_LC_LZ2(0x088000, rom, GFX_32_buffer, 0x8000);
	decompress_LC_LZ2(0x08BFC0, rom, GFX_33_buffer, 0x8000);

	buffer temp_buffer;
	// Upload GFX32 (already 4bpp):
	//convert_3bpp_to_4bpp(GFX_32_buffer, temp_buffer);
	animation_emulator.upload_to_ram(GFX_32_buffer, 0x7E2000);

	// Upload GFX33:
	convert_3bpp_to_4bpp(GFX_33_buffer, temp_buffer);
	qWarning() << "buffer_length:" << temp_buffer.get_length();
	animation_emulator.upload_to_ram(temp_buffer, 0x7E7D00);
}

void update_vanilla_gfx_animation(object_emulator & animation_emulator, unsigned char frame_counter)
{
	// Simulate the updating of the tiles to be animated:
	animation_emulator.emulate_gfx_animation(0x05BB39, frame_counter);

	// Simulate the uploading of the tiles to be animated:
	animation_emulator.emulate_gfx_animation(0x00A390, frame_counter);
}
