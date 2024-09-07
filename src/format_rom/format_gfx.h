#ifndef FORMAT_GFX_H
#define FORMAT_GFX_H

#include "main.h"

struct gfx_animation_queue_item
{
	unsigned int destination_x_position;
	unsigned int destination_y_position;

	unsigned int source_address;
};

class gfx_animation_queue
{
public:
	gfx_animation_queue();
};

#endif // FORMAT_GFX_H
