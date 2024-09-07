#include "rats_viewer_widget.h"

#include "utilities/queue.h"

#include <QDebug>

rats_viewer_widget::rats_viewer_widget(SMW_ROM &rom, QWidget *parent) :
	opened_rom(rom),

	central_layout(parent),
	main_splitter(parent),
	view_rats_list(&main_splitter)
{
	setup_ui();

	view_rats_list.setModel(&rats_tag_item_list);
}

void rats_viewer_widget::setup_ui()
{
	central_layout.setSpacing(0);
	central_layout.setContentsMargins(0, 0, 0, 0);

	main_splitter.setOrientation(Qt::Horizontal);

	main_splitter.addWidget(&view_rats_list);
	central_layout.addWidget(&main_splitter,0,0);
	this->setLayout(&central_layout);
}

void rats_viewer_widget::show_toolbar_items(QToolBar & toolbar, QWidget *parent)
{
	is_active = true;

	display_rats_tags();
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

	display_rats_tags();
}

void rats_viewer_widget::display_rats_tags()
{
	opened_rom.scan_for_rats_tags();

	rats_tag_item_list.clear();

	// Fill the object types list (in the 'Insert' menu and 'Object Settings' setting):
	for (int a=0; a<opened_rom.rats_tag_list.get_length(); a++) {

		opened_rom.rats_tag_list.get_item(a).location;
		rats_block new_block = rats_block(opened_rom.rats_tag_list.get_item(a), opened_rom);

		if(new_block.is_rpns_format == false)
		{
			// This can be 'new', since 'delete' will be called on removeRow anyway. This is not a memory leak.
			rats_tag_item_list.appendRow(new QStandardItem(QString("%1").arg(a, 2, 16, QLatin1Char( '0' )).toUpper() + ": Raw RATS tag at: 0x"
														   + QString("%1").arg(opened_rom.rats_tag_list.get_item(a).location.to_snes_format(opened_rom.has_header), 6, 16, QLatin1Char( '0' )).toUpper()
														   + ", "
														   + QString("%1").arg(opened_rom.rats_tag_list.get_item(a).allocation_size, 1, 10, QLatin1Char( '0' )).toUpper()
														   + " bytes long."
														   ));
			rats_tag_item_list.item(a)->removeRows(0, rats_tag_item_list.item(a)->rowCount());
		} else {
			// This can be 'new', since 'delete' will be called on removeRow anyway. This is not a memory leak.
			rats_tag_item_list.appendRow(new QStandardItem(QString("%1").arg(a, 2, 16, QLatin1Char( '0' )).toUpper() + ": Untitled RPNS-RATS tag at: 0x"
														   + QString("%1").arg(opened_rom.rats_tag_list.get_item(a).location.to_snes_format(opened_rom.has_header), 6, 16, QLatin1Char( '0' )).toUpper()
														   + ", "
														   + QString("%1").arg(opened_rom.rats_tag_list.get_item(a).allocation_size, 1, 10, QLatin1Char( '0' )).toUpper()
														   + " bytes long , containing "
														   + QString("%1").arg(new_block.get_length(), 1, 10, QLatin1Char( '0' )).toUpper()
														   + " partitions."
														   ));
			rats_tag_item_list.item(a)->removeRows(0, rats_tag_item_list.item(a)->rowCount());

			for(int b=0; b<(int)new_block.get_length(); b++)
			{
				rats_tag_item_list.item(a)->appendRow(new QStandardItem(QString("%1").arg(b, 2, 16, QLatin1Char( '0' )).toUpper() + ": "
																		+ partition_type_tostring(new_block.get_partition_type(b))
																		+ " Partition."
																		));
			}

		}



	}
}
