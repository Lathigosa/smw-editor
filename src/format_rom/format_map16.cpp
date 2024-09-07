#include "main.h"

map16_page::map16_page()
{
	map = new map16_gfx_tile[256];
}

map16_page::~map16_page()
{
	delete[] map;
}

void map16_page::setBlock(unsigned char index, const map16_gfx_tile &data)
{
	map[index].act_as_hi = data.act_as_hi;
	map[index].act_as_lo = data.act_as_lo;

	unsigned int a;
	for (a=0;a<4;a++)
	{
		map[index].gfx_tiles[a].low_byte = data.gfx_tiles[a].low_byte;
		map[index].gfx_tiles[a].high_byte = data.gfx_tiles[a].high_byte;

		map[index].gfx_tiles[a].priority = data.gfx_tiles[a].priority;
		map[index].gfx_tiles[a].palette = data.gfx_tiles[a].palette;

		map[index].gfx_tiles[a].flip_x = data.gfx_tiles[a].flip_x;
		map[index].gfx_tiles[a].flip_y = data.gfx_tiles[a].flip_y;
	}
}

map16_gfx_tile & map16_page::getBlock(unsigned char index) const
{
	return map[index];
}


// ====================================== //

map16_map::map16_map(unsigned int amount_of_pages)
{
	page_count = amount_of_pages;
	map = new map16_page[amount_of_pages];
}

map16_map::~map16_map()
{
	delete[] map;
}

void map16_map::setBlock(unsigned char page, unsigned char index, const map16_gfx_tile &data)
{
	if (page >= page_count)
		return;

	map[page].setBlock(index, data);
}

map16_gfx_tile map16_map::getBlock(unsigned char page, unsigned char index) const
{
	if (page >= page_count)
	{
		return map16_gfx_tile();
	}

	return map[page].getBlock(index);
}

unsigned int map16_map::getPageCount() const
{
	return page_count;
}
