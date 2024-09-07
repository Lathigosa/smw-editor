#include "format_rom/format_level/format_level_screen_exit.h"

screen_exit_list::screen_exit_list()
{
	for(int a=0; a<32; a++)
	{
		buffer[a] = screen_exit();
	}
}

void screen_exit_list::set_screen_exit(unsigned int index, const screen_exit & data)
{
	if(index >= 32)
		return;

	buffer[index] = data;
}

const screen_exit screen_exit_list::get_screen_exit(unsigned int index)
{
	if(index >= 32)
		return screen_exit();

	return buffer[index];
}
