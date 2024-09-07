#ifndef LOAD_ROM
#define LOAD_ROM

#include "utilities/address.h"
#include "utilities/buffer.h"
#include "utilities/queue.h"
#include "allocation.h"
//#include "main.h"


enum struct file_error
{
	file_ok = 0,
	file_genericerror = 1,
	file_namenotset = 2,
	file_notsmw = 3
};

namespace file_error_string {
  static const char* string[] = {
	  "No Error",
	  "Generic Error",
	  "Name Not Set",
	  "File Type Not Recognized"
  };

  static const char* string_long[] = {
	  "There is no error.",
	  "A generic error occurred.",
	  "The file name is not set! This is a bug.",
	  "The file is not recognized as a proper SMW ROM."
  };
}

enum struct rom_size
{
	banks_16 = 0x09,	// 0.5 MB
	banks_32 = 0x0A,	// 1.0 MB
	banks_64 = 0x0B,	// 2.0 MB
	banks_128 = 0x0C	// 4.0 MB
};

class SMW_ROM
{
public:
	SMW_ROM();
	~SMW_ROM();

	file_error set_filename(const char *filename);
	file_error load_rom();
	file_error save_rom();
	void expand_rom(rom_size size);

	unsigned char get_byte(snes_address address) const;
	unsigned char get_byte_pc(unsigned int pc_address) const;
	void set_byte(snes_address address, unsigned char data);
	void set_byte_pc(unsigned int pc_address, unsigned char data);

	snes_address find_unprotected_freespace(int data_size, unsigned char bank, unsigned char empty_byte = 0x00);
	pc_address find_rats_freespace(int data_size);

	rats_tag allocate_rats(const rats_block &data);
	bool deallocate_rats(rats_tag tag);

	void scan_for_rats_tags();					// Scan the entire ROM file for rats tags and fill the list.

	queue<rats_tag> rats_tag_list;		// The list of all rats tags in the ROM.

	unsigned int get_length() const
	{
		return rom_buffer.get_length();
	}

	bool has_header;			// This bool is <true> if the ROM was opened with a 0x200 (512) byte header and <false> if not.

	bool is_open() const;
private:
	bool is_name_set;			// <true> if OpenFilename has been called succesfully at least once.
	bool buffer_exists;			// <true> if the buffer isn't empty.
	buffer opened_filename;		// The filename of the currently opened ROM.
	bool is_locked;				// Set to <true> if the ROM is being edited outside of the program and should be reloaded.

	buffer rom_buffer;			// The buffer containing all the data. Each save or load-operation reloads this buffer from disk, so that other programs can edit it too.


};

#endif // LOAD_ROM
