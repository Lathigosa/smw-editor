#ifndef FORMAT_LEVEL_SCREEN_EXIT_H
#define FORMAT_LEVEL_SCREEN_EXIT_H

struct screen_exit {
	unsigned char page_number = 0;
	bool is_water_level = false;
	bool is_secondary_exit = false;
	unsigned short destination_level = 0;					// In an unchanged ROM, this value is limited to sizeof(unsigned char).

	bool use_high_destination_bit = false;					// Used in Lunar Magic.
	bool high_bit = false;
};

class screen_exit_list {
public:
	screen_exit_list();

	void set_screen_exit(unsigned  int index, const screen_exit & data);
	const screen_exit get_screen_exit(unsigned int index);

private:
	screen_exit buffer[32];
};

#endif // FORMAT_LEVEL_SCREEN_EXIT_H
