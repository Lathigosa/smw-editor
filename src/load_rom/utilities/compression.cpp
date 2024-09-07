#include "main.h"

// Decompresses any file with the LC_LZ2 format. Returns the decompressed buffer.
// max_size indicates how large you expect the decompressed file to be. Make it
//   as large or slightly larger than the file you expect, but not too large,
//   because it will create a buffer of that size (smaller is better).
// If the file is larger than max_size, the function will return <false> and won't
//   output a buffer.
// If everything goes right, it returns <true>.
bool decompress_LC_LZ2(snes_address address, const SMW_ROM &rom, buffer &decompression_buffer, int max_size)
{
	buffer process_buffer(max_size);

	// We use a ROM pointer in pc format for performance reasons,
	// and because the LC_LZ2 compression is capable of crossing
	// banks. Using snes_address would only lead to more hassle.
	unsigned int pc_pointer = address.to_pc_format(rom.has_header);			// The current address processed within the ROM, in pc format because of performance reasons.

	unsigned int current_command;											// The command currently being processed.
	unsigned int current_length;											// The length component of the current chunk header.
	unsigned int current_length_high;
	unsigned int long_length;

	// Repeat Mode:
	unsigned int high_byte;
	unsigned int low_byte;
	unsigned int address_to_copy;


	int a = 0;
	int b;
	bool firstbyte = true;
	while(a<max_size)
	{
		current_command = rom.get_byte_pc(pc_pointer);

		if(current_command == 0xFF)											// If the END signal is given by the compression, stop decompressing.
			break;

		current_command = rom.get_byte_pc(pc_pointer) & 0xE0;					// Mask out 0b11100000 to reveal only the command bits.
		current_length = (rom.get_byte_pc(pc_pointer) & 0x1F) + 1;			// Mask out 0b00011111 to reveal only the length bits.
		long_length = current_length;

		if (current_command == 0xE0) {										// If the command is a long length command.
			current_command = (rom.get_byte_pc(pc_pointer) << 3)  & 0xE0;		// Bitshift the byte left by three to get the real command.
			current_length_high = rom.get_byte_pc(pc_pointer) & 0x03;
			pc_pointer++;
			current_length = rom.get_byte_pc(pc_pointer);
			long_length = ((current_length_high << 8) | current_length) + 1;
		}

		switch ( current_command ) {
		case 0x00:	//000 "Direct Copy"
			for (b=0;b<long_length;b++) {
				pc_pointer++;								// Increment address.
				process_buffer.set_byte(a, rom.get_byte_pc(pc_pointer));
				a++;										// Increment buffer position.
			}
			break;
		case 0x20:	//001 "Byte Fill"
			pc_pointer++;									// Increment address once.
			for (b=0;b<long_length;b++) {
				process_buffer.set_byte(a, rom.get_byte_pc(pc_pointer));
				a++;										// Increment buffer position.
			}
			break;
		case 0x40:	//010 "Word Fill"
			pc_pointer++;									// Increment address once.
			for (b=0;b<long_length;b++) {
				if (firstbyte == true) {
					firstbyte = false;

					process_buffer.set_byte(a, rom.get_byte_pc(pc_pointer));
					a++;									// Increment buffer position.

				} else {
					firstbyte = true;

					process_buffer.set_byte(a, rom.get_byte_pc(pc_pointer + 1));
					a++;									// Increment buffer position.

				}
			}
			pc_pointer++;							// Increment address.
			firstbyte = true;								// Reset firstbyte for next command.
			break;
		case 0x60:	//011 "Increasing Fill"
			pc_pointer++;									// Increment address once.
			for (b=0;b<long_length;b++) {
				process_buffer.set_byte(a, rom.get_byte_pc(pc_pointer) + b);
				a++;										// Increment buffer position.
			}
			break;
		case 0x80:	//100 "Repeat"
			pc_pointer++;
			high_byte = rom.get_byte_pc(pc_pointer);

			pc_pointer++;
			low_byte = rom.get_byte_pc(pc_pointer);

			address_to_copy = (high_byte << 8) | low_byte;

			for (b=0;b<long_length;b++) {
				process_buffer.set_byte(a, process_buffer.get_byte(address_to_copy));
				address_to_copy++;
				a++;
			}
			break;
		default:
			qWarning() << "Unknown Code!" << current_command;
			break;
		}
		pc_pointer++;										// Continue to the next address.

		//pc_address pointer_2 = pc_pointer;

		//qWarning() << "pc_pointer:" << pointer_2.to_snes_format(rom.has_header) << "Current Code:" << current_command << " With length: " << long_length;
	}

	qWarning() << "Total amount of commands in LC_LZ2 decompression:" << a ;

	// Initialize new buffer with the proper size:
	buffer temp_buffer(a);
	temp_buffer.import_buffer(process_buffer);
	decompression_buffer = temp_buffer;


	return true;
}

