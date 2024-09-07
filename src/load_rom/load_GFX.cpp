#include "main.h"

error_gfx convert_3bpp_to_4bpp(buffer &GFX_buffer, buffer &output_buffer)
{
	// Check for any file errors first:
	if ((GFX_buffer.get_length() % 24) != 0)		// If the remainder of the length divided by 24 isn't 0, there's a pixel mismatch.
		return pixel_mismatch;				// BPP_3 should have three bytes per row of eight pixels, so 24 bytes per tile [3 bytes * 8 lines].

	// Set the size of the output buffer to the proper value for a BPP_4 file:
	output_buffer = buffer((GFX_buffer.get_length() *4) /3);
	output_buffer.clear_buffer();

	unsigned int old_address_counter = 0;			// The index counter of the compressed buffer.
	unsigned int new_address_counter = 0;			// The index counter of the new buffer.

	int a;
	int b;
	for (a=0; a<(GFX_buffer.get_length() / (3 * 8)); a++) {
		for (b=0; b<16; b++) {
			// Copy the first 16 bytes (bit planes 0 and 1):
			output_buffer.set_byte(new_address_counter, GFX_buffer.get_byte(old_address_counter));
			old_address_counter++;
			new_address_counter++;
		}

		for (b=0; b<8; b++) {
			// Copy the last byte and fill the other one with 0x00 (bit planes 2 and 3):
			output_buffer.set_byte(new_address_counter, GFX_buffer.get_byte(old_address_counter));
			old_address_counter++;
			new_address_counter++;

			// The fourth byte (bit plane 3) is always zero, since that's the missing information we add:
			output_buffer.set_byte(new_address_counter, 0x00);
			new_address_counter++;
		}
	}
	return success;
}

void gfx_decode_get_one_bits(buffer & decompressed_buffer, unsigned int offset, unsigned char byte_1, int shift_bits_left)
{
	decompressed_buffer.set_byte(offset + 7, decompressed_buffer.get_byte(offset + 7) | (((byte_1 & 0x01)) << shift_bits_left));
	decompressed_buffer.set_byte(offset + 6, decompressed_buffer.get_byte(offset + 6) | (((byte_1 & 0x02) >> 1) << shift_bits_left));
	decompressed_buffer.set_byte(offset + 5, decompressed_buffer.get_byte(offset + 5) | (((byte_1 & 0x04) >> 2) << shift_bits_left));
	decompressed_buffer.set_byte(offset + 4, decompressed_buffer.get_byte(offset + 4) | (((byte_1 & 0x08) >> 3) << shift_bits_left));
	decompressed_buffer.set_byte(offset + 3, decompressed_buffer.get_byte(offset + 3) | (((byte_1 & 0x10) >> 4) << shift_bits_left));
	decompressed_buffer.set_byte(offset + 2, decompressed_buffer.get_byte(offset + 2) | (((byte_1 & 0x20) >> 5) << shift_bits_left));
	decompressed_buffer.set_byte(offset + 1, decompressed_buffer.get_byte(offset + 1) | (((byte_1 & 0x40) >> 6) << shift_bits_left));
	decompressed_buffer.set_byte(offset + 0, decompressed_buffer.get_byte(offset + 0) | (((byte_1 & 0x80) >> 7) << shift_bits_left));
}

void gfx_decode_get_two_bits(buffer & decompressed_buffer, unsigned int offset, unsigned char byte_1, unsigned char byte_2, int shift_bits_left)
{
	decompressed_buffer.set_byte(offset + 7, decompressed_buffer.get_byte(offset + 7) | (((byte_1 & 0x01)      | (byte_2 & 0x01) << 1) << shift_bits_left));
	decompressed_buffer.set_byte(offset + 6, decompressed_buffer.get_byte(offset + 6) | (((byte_1 & 0x02) >> 1 | (byte_2 & 0x02)) << shift_bits_left));
	decompressed_buffer.set_byte(offset + 5, decompressed_buffer.get_byte(offset + 5) | (((byte_1 & 0x04) >> 2 | (byte_2 & 0x04) >> 1) << shift_bits_left));
	decompressed_buffer.set_byte(offset + 4, decompressed_buffer.get_byte(offset + 4) | (((byte_1 & 0x08) >> 3 | (byte_2 & 0x08) >> 2) << shift_bits_left));
	decompressed_buffer.set_byte(offset + 3, decompressed_buffer.get_byte(offset + 3) | (((byte_1 & 0x10) >> 4 | (byte_2 & 0x10) >> 3) << shift_bits_left));
	decompressed_buffer.set_byte(offset + 2, decompressed_buffer.get_byte(offset + 2) | (((byte_1 & 0x20) >> 5 | (byte_2 & 0x20) >> 4) << shift_bits_left));
	decompressed_buffer.set_byte(offset + 1, decompressed_buffer.get_byte(offset + 1) | (((byte_1 & 0x40) >> 6 | (byte_2 & 0x40) >> 5) << shift_bits_left));
	decompressed_buffer.set_byte(offset + 0, decompressed_buffer.get_byte(offset + 0) | (((byte_1 & 0x80) >> 7 | (byte_2 & 0x80) >> 6) << shift_bits_left));
}

// Loads a GFX file from a buffer into a texture.
// Use this if you want to show the GFX file on-screen or
//   want to manipulate it using shaders.
// For normal GFX files (GFX00-GFX31), the v_tile_offset should be 8 * page_in_texture.
error_gfx load_gfx_to_texture(buffer &GFX_buffer, unsigned int buffer_offset, unsigned int buffer_length, BPP bits_per_pixel, GLuint texture, unsigned int x_tile_size, unsigned int y_tile_size, unsigned int x_tile_offset, unsigned int y_tile_offset)
{
	// Test texture dimensions:
	glBindTexture(GL_TEXTURE_2D, texture);

	int w, h;
	glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_WIDTH, &w);
	glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_HEIGHT, &h);

	if (w < (x_tile_size * 8 + x_tile_offset * 8))
		return texture_too_small;

	if (h < (y_tile_size * 8 + y_tile_offset * 8))
		return texture_too_small;

	if (w % 8 != 0)
		return texture_size_mismatch;

	if (h % 8 != 0)
		return texture_size_mismatch;

	// Set the length of the piece of data to upload to the texture:
	unsigned int length;
	if (buffer_length == 0)
		length = GFX_buffer.get_length();
	else
		length = buffer_length;

	// Select the proper buffer size:
	int buffer_size;

	if ((length % (8 * bits_per_pixel)) != 0)					// If the remainder of the length divided by 16 isn't 0, there's a pixel mismatch.
		return pixel_mismatch;									// BPP_2 should have two bytes per row of eight pixels, so 16 bytes per tile [2 bytes * 8 lines].

	buffer_size = (length / (int)bits_per_pixel) * 8;

	buffer new_buffer(buffer_size);

	unsigned int old_address_counter = buffer_offset;			// The index counter of the compressed buffer.
	unsigned int new_address_counter = 0;						// The index counter of the new buffer.



	unsigned char byte_1;
	unsigned char byte_2;

	int a;
	int b;

	switch ( bits_per_pixel ) {
	case BPP_2:
		if ((length % 16) != 0)									// If the remainder of the length divided by 16 isn't 0, there's a pixel mismatch.
			return pixel_mismatch;								// BPP_2 should have two bytes per row of eight pixels, so 16 bytes per tile [2 bytes * 8 lines].

		new_buffer.clear_buffer();

		for (a=0; a<(length / (2 * 8)); a++) {
			for (b=0; b<8; b++) {
				// Get next two bytes:
				byte_1 = GFX_buffer.get_byte(old_address_counter);
				old_address_counter++;
				byte_2 = GFX_buffer.get_byte(old_address_counter);
				old_address_counter++;

				gfx_decode_get_two_bits(new_buffer, new_address_counter, byte_1, byte_2, 0);
				new_address_counter += 8;
			}
		}

		break;
	case BPP_3:
		if ((length % 24) != 0)									// If the remainder of the length divided by 24 isn't 0, there's a pixel mismatch.
			return pixel_mismatch;								// BPP_3 should have three bytes per row of eight pixels, so 24 bytes per tile [3 bytes * 8 lines].

		new_buffer.clear_buffer();

		for (a=0; a<(length / (3 * 8)); a++) {
			for (b=0; b<8; b++) {
				// Get next two bytes:
				byte_1 = GFX_buffer.get_byte(old_address_counter);
				old_address_counter++;
				byte_2 = GFX_buffer.get_byte(old_address_counter);
				old_address_counter++;

				// Fill the first two bits of each byte in the array:
				gfx_decode_get_two_bits(new_buffer, new_address_counter, byte_1, byte_2, 0);
				new_address_counter += 8;
			}
			new_address_counter -= 64;
			for (b=0; b<8; b++) {
				// Get next byte:
				byte_1 = GFX_buffer.get_byte(old_address_counter);
				old_address_counter++;

				// Get the next bit of each byte in the array:
				gfx_decode_get_one_bits(new_buffer, new_address_counter, byte_1, 2);
				new_address_counter += 8;
			}
		}

		break;
	case BPP_4:
		if ((length % 32) != 0)									// If the remainder of the length divided by 32 isn't 0, there's a pixel mismatch.
			return pixel_mismatch;								// BPP_4 should have four bytes per row of eight pixels, so 32 bytes per tile [4 bytes * 8 lines].

		new_buffer.clear_buffer();

		for (a=0; a<(length / (4 * 8)); a++) {
			for (b=0; b<8; b++) {
				// Get next two bytes:
				byte_1 = GFX_buffer.get_byte(old_address_counter);
				old_address_counter++;
				byte_2 = GFX_buffer.get_byte(old_address_counter);
				old_address_counter++;

				// Fill the first two bits of each byte in the array:
				gfx_decode_get_two_bits(new_buffer, new_address_counter, byte_1, byte_2, 0);
				new_address_counter += 8;
			}
			new_address_counter -= 64;
			for (b=0; b<8; b++) {
				// Get next two bytes:
				byte_1 = GFX_buffer.get_byte(old_address_counter);
				old_address_counter++;
				byte_2 = GFX_buffer.get_byte(old_address_counter);
				old_address_counter++;

				// Fill the first two bits of each byte in the array:
				gfx_decode_get_two_bits(new_buffer, new_address_counter, byte_1, byte_2, 2);
				new_address_counter += 8;
			}
		}

		break;
	case BPP_8:
		if ((length % 64) != 0)									// If the remainder of the length divided by 64 isn't 0, there's a pixel mismatch.
			return pixel_mismatch;								// BPP_8 should have eight bytes per row of eight pixels, so 64 bytes per tile [8 bytes * 8 lines].

		new_buffer.clear_buffer();

		for (a=0; a<(length / (8 * 8)); a++) {
			for (b=0; b<8; b++) {
				// Get next two bytes:
				byte_1 = GFX_buffer.get_byte(old_address_counter);
				old_address_counter++;
				byte_2 = GFX_buffer.get_byte(old_address_counter);
				old_address_counter++;

				// Fill the first two bits of each byte in the array:
				gfx_decode_get_two_bits(new_buffer, new_address_counter, byte_1, byte_2, 0);
				new_address_counter += 8;
			}
			new_address_counter -= 64;
			for (b=0; b<8; b++) {
				// Get next two bytes:
				byte_1 = GFX_buffer.get_byte(old_address_counter);
				old_address_counter++;
				byte_2 = GFX_buffer.get_byte(old_address_counter);
				old_address_counter++;

				// Fill the first two bits of each byte in the array:
				gfx_decode_get_two_bits(new_buffer, new_address_counter, byte_1, byte_2, 2);
				new_address_counter += 8;
			}
			new_address_counter -= 64;
			for (b=0; b<8; b++) {
				// Get next two bytes:
				byte_1 = GFX_buffer.get_byte(old_address_counter);
				old_address_counter++;
				byte_2 = GFX_buffer.get_byte(old_address_counter);
				old_address_counter++;

				// Fill the first two bits of each byte in the array:
				gfx_decode_get_two_bits(new_buffer, new_address_counter, byte_1, byte_2, 4);
				new_address_counter += 8;
			}
			new_address_counter -= 64;
			for (b=0; b<8; b++) {
				// Get next two bytes:
				byte_1 = GFX_buffer.get_byte(old_address_counter);
				old_address_counter++;
				byte_2 = GFX_buffer.get_byte(old_address_counter);
				old_address_counter++;

				// Fill the first two bits of each byte in the array:
				gfx_decode_get_two_bits(new_buffer, new_address_counter, byte_1, byte_2, 6);
				new_address_counter += 8;
			}
		}

		break;
	default:

		break;
	}

	// Now the file is stored in 8x8 sections.

	int tile_count_total = new_buffer.get_length() / 64;		// Total amount of tiles to be stored.

	int x_pixel_offset = x_tile_offset * 8;
	int y_pixel_offset = y_tile_offset * 8;

	for (a = 0; a < tile_count_total; a++) {
		glTexSubImage2D(GL_TEXTURE_2D,
			0,
			x_pixel_offset,
			y_pixel_offset,
			8,
			8,
			GL_RED,
			GL_UNSIGNED_BYTE,
			reinterpret_cast<void*>(&new_buffer.getBuffer()[a * 64]));
		x_pixel_offset += 8;
		if(x_pixel_offset == x_tile_size * 8 + x_tile_offset * 8) {
			y_pixel_offset += 8;
			x_pixel_offset = x_tile_offset * 8;
		}
	}

	return success;
}

// TODO: Expand the next function to load BPP_4 pages as well!
error_gfx load_gfx_page(const SMW_ROM & rom, GLuint texture, unsigned char gfx_page)
{
	unsigned char address_low = rom.get_byte(0x00B992 + gfx_page);
	unsigned char address_high = rom.get_byte(0x00B9C4 + gfx_page);
	unsigned char address_bank = rom.get_byte(0x00B9F6 + gfx_page);
	unsigned int address = address_low | (address_high << 8) | (address_bank << 16);

	// Decompress the GFX file (expect the size not to exceed 4096):
	buffer decompression_buffer;

	decompress_LC_LZ2(address, rom, decompression_buffer, 4096);

	BPP bits_per_pixel = BPP_4;

	if(decompression_buffer.get_length() == 4096)
		bits_per_pixel = BPP_4;
	else if(decompression_buffer.get_length() == 3072)
		bits_per_pixel = BPP_3;
	else if(decompression_buffer.get_length() == 2048)
		bits_per_pixel = BPP_2;
	else
		return pixel_mismatch;

	return load_gfx_to_texture(decompression_buffer, 0, 0, bits_per_pixel, texture, 16, 8, 0, 0);
}

void load_vanilla_gfx(const SMW_ROM & rom, GLuint terrain_texture, GLuint sprite_texture, level_header header)
{
	// TODO: make sure that the GFX list isn't hardcoded, so it can be expanded.
	// The address only gets referenced from CODE_00BA2C and two codes afterwards,
	// so that's where we can grab the address to the list.

	// Get the terrain gfx:
	{
		// Get the GFX numbers to use, based on the tileset number:
		unsigned char GFX_list[4] =
			{ rom.get_byte_pc(PC_ADDRESS(0x00A92B + header.tile_set * 4, rom.has_header)),
			  rom.get_byte_pc(PC_ADDRESS(0x00A92C + header.tile_set * 4, rom.has_header)),
			  rom.get_byte_pc(PC_ADDRESS(0x00A92D + header.tile_set * 4, rom.has_header)),
			  rom.get_byte_pc(PC_ADDRESS(0x00A92E + header.tile_set * 4, rom.has_header)) };

		buffer decompression_buffer;

		unsigned int a;
		for(a=0;a<4;a++)
		{
			// Get addresses of the GFX file:
			unsigned char address_low = rom.get_byte_pc(PC_ADDRESS(0x00B992 + GFX_list[a], rom.has_header));
			unsigned char address_high = rom.get_byte_pc(PC_ADDRESS(0x00B9C4 + GFX_list[a], rom.has_header));
			unsigned char address_bank = rom.get_byte_pc(PC_ADDRESS(0x00B9F6 + GFX_list[a], rom.has_header));
			unsigned int address = address_low | (address_high << 8) | (address_bank << 16);

			// Decompress the GFX file (expect the size not to exceed 4096):
			decompress_LC_LZ2(address, rom, decompression_buffer, 4096);
			qWarning() << "Total Length of decompression buffer:" << decompression_buffer.get_length();
			load_gfx_to_texture(decompression_buffer, 0, 0, BPP_3, terrain_texture, 16, 8, 0, a*8);
		}
	}

	// Get the sprite gfx:
	{
		// Get the GFX numbers to use, based on the tileset number:
		unsigned char GFX_list[4] =
			{ rom.get_byte_pc(PC_ADDRESS(0x00A8C3 + header.sprite_set * 4, rom.has_header)),
			  rom.get_byte_pc(PC_ADDRESS(0x00A8C4 + header.sprite_set * 4, rom.has_header)),
			  rom.get_byte_pc(PC_ADDRESS(0x00A8C5 + header.sprite_set * 4, rom.has_header)),
			  rom.get_byte_pc(PC_ADDRESS(0x00A8C6 + header.sprite_set * 4, rom.has_header)) };

		buffer decompression_buffer;

		unsigned int a;
		for(a=0;a<4;a++)
		{
			// Get addresses of the GFX file:
			unsigned char address_low = rom.get_byte_pc(PC_ADDRESS(0x00B992 + GFX_list[a], rom.has_header));
			unsigned char address_high = rom.get_byte_pc(PC_ADDRESS(0x00B9C4 + GFX_list[a], rom.has_header));
			unsigned char address_bank = rom.get_byte_pc(PC_ADDRESS(0x00B9F6 + GFX_list[a], rom.has_header));
			unsigned int address = address_low | (address_high << 8) | (address_bank << 16);

			// Decompress the GFX file (expect the size not to exceed 4096):
			decompress_LC_LZ2(address, rom, decompression_buffer, 4096);
			qWarning() << "Total Length of decompression buffer:" << decompression_buffer.get_length();
			load_gfx_to_texture(decompression_buffer, 0, 0, BPP_3, sprite_texture, 16, 8, 0, a*8);
		}
	}
}
