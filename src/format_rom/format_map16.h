#ifndef FORMAT_MAP16_H
#define FORMAT_MAP16_H

struct map8_gfx_tile
{
	unsigned char low_byte;			// Format: top left, bottom left, top right, bottom right.
	unsigned char high_byte;

	bool priority;
	bool flip_x;
	bool flip_y;

	unsigned char palette;
};

struct map16_gfx_tile
{
	map8_gfx_tile gfx_tiles[4];

	unsigned char act_as_lo;
	unsigned char act_as_hi;
};


class map16_page
{
public:
	map16_page();
	~map16_page();

	map16_gfx_tile &getBlock(unsigned char index) const;
	void setBlock(unsigned char index, const map16_gfx_tile &data);

private:
	map16_gfx_tile * map;
};


class map16_map
{
public:
	map16_map(unsigned int amount_of_pages);
	~map16_map();

	map16_gfx_tile getBlock(unsigned char page, unsigned char index) const;
	void setBlock(unsigned char page, unsigned char index, const map16_gfx_tile &data);

	unsigned int getPageCount() const;

private:
	unsigned char page_count;
	map16_page * map;
};

#endif // FORMAT_MAP16_H
