#include "main.h"

// load_file - Returns a buffer containing the newly opened file in byte format.
// *filename: the path to the file to open.
// &length: the size of file returned in fSize.
buffer load_file(const char *filename)
{
	std::FILE *fp = std::fopen(filename, "rb");
	if (fp)
	{
		unsigned char * contents;
		std::fseek(fp, 0, SEEK_END);
		int size = std::ftell(fp);
		contents = new unsigned char[size];
		std::rewind(fp);
		std::fread(&contents[0], 1, size, fp);
		std::fclose(fp);

		buffer new_buffer(size);
		for(int a=0; a<size; a++)
			new_buffer[a] = contents[a];

		return(new_buffer);
	}
	return buffer();
}

// save_file - Saves a buffer to the file system.
// *filename: the path to the file to write to.
// &data: the buffer containing the data to be written.
bool save_file(const char *filename, buffer &data)
{
	std::FILE *fp = std::fopen(filename, "wb");
	if (fp)
	{
		std::fwrite(data.getBuffer(), 1, data.get_length(), fp);
		std::fclose(fp);

		return true;
	}
	return false;
}

