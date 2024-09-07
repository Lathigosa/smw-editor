#ifndef ENCODE_LEVEL_H
#define ENCODE_LEVEL_H

#include "format_rom/format_level.h"
#include "utilities/buffer.h"
#include "utilities/queue.h"

buffer encode_level(queue<level_object> & object_list, level_header & header);

#endif // ENCODE_LEVEL_H
