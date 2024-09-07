#ifndef LOAD_FILE_H
#define LOAD_FILE_H

#include "utilities/buffer.h"

// loadFile - loads text file into char* fname
// allocates memory - so need to delete after use
// size of file returned in fSize
buffer load_file(const char *filename);

// save_file - Saves a buffer to the file system.
// *filename: the path to the file to write to.
// &data: the buffer containing the data to be written.
bool save_file(const char *filename, buffer &data);

#endif // LOAD_FILE_H
