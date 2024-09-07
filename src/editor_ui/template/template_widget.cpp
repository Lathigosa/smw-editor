#include "rats_viewer_widget.h"

rats_viewer_widget::rats_viewer_widget(SMW_ROM &rom, QWidget *parent) :
	opened_rom(rom)
{

}

void rats_viewer_widget::show_toolbar_items(QToolBar & toolbar, QWidget *parent)
{
	is_active = true;

}

void rats_viewer_widget::stop_animation()
{
	//level_editor.stop_animation();
	//object_viewer.stop_animation();

	is_active = false;
}

void rats_viewer_widget::start_animation()
{
	//if(opened_rom.is_open() == false)
	//{
	//	emit emit_message(message_type::warning, "No ROM file loaded yet!");
	//	return;
	//}

	//level_editor.start_animation(opened_rom, opened_level.header.tile_set);

	is_active = true;
}

void rats_viewer_widget::on_rom_load()
{
	// Update all object view widgets, because the rom has reloaded:
	//select_level(level_selector->value());
	//on_insert_object_list_selection_changed(insert_object_list.selectionModel()->currentIndex(), insert_object_list.selectionModel()->currentIndex());
}
