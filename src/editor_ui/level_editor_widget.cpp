#include "editor_ui/level_editor_widget.h"

#define LAYER_1			1
#define LAYER_2			0
#define LEVEL_LAYER_1	0
#define LEVEL_LAYER_2	1

level_editor_widget::level_editor_widget(SMW_ROM &rom, QWidget *parent) :
	opened_rom(rom),
	QWidget(parent),
	level_editor(parent),
	object_viewer(parent, &level_editor),
	level_selector(new HexSpinBox(parent)),
	zoom_slider(new QSlider(Qt::Horizontal, parent)),
	opened_map16(2),

	central_layout(parent),
	main_splitter(parent),
	insert_panel(&main_splitter),
	insert_panel_layout(&insert_panel),
	insert_panel_title(&insert_panel),
	insert_object_list(&insert_panel),
	insert_object_details(&insert_panel),
	object_details_label(&insert_object_details),
	insert_object_details_layout(&insert_object_details),

	editor_panel(&main_splitter),
	editor_panel_layout(&editor_panel),
	editor_panel_v_scrollbar(&editor_panel),
	editor_panel_h_scrollbar(&editor_panel)
{
	setup_ui();

	insert_object_list.setModel(&object_types_items);

	editor_panel_v_scrollbar.connect(&editor_panel_v_scrollbar, SIGNAL(valueChanged(int)), this, SLOT(on_v_scrollbar_slider_moved(int)));
	editor_panel_h_scrollbar.connect(&editor_panel_h_scrollbar, SIGNAL(valueChanged(int)), this, SLOT(on_h_scrollbar_slider_moved(int)));

	start_object_editor();

	main_splitter.connect(&main_splitter, SIGNAL(splitterMoved(int,int)), this, SLOT(on_main_splitter_moved(int, int)));

	insert_object_list.selectionModel()->connect(insert_object_list.selectionModel(), SIGNAL(currentChanged(QModelIndex,QModelIndex)), this, SLOT(on_insert_object_list_selection_changed(const QModelIndex &, const QModelIndex &)));

	level_selector->setRange(0x00, 0x1FF);
	level_selector->setValue(0x00);

	zoom_slider->setRange(0, 4);
	zoom_slider->setValue(2);
	zoom_slider->setMaximumWidth(100);

	clear_object_viewer();
}

void level_editor_widget::setup_ui()
{
	setAutoFillBackground(true);

	level_editor.setBackgroundRole(QPalette::Base);

	central_layout.setSpacing(0);
	central_layout.setContentsMargins(0, 0, 0, 0);

	main_splitter.setOrientation(Qt::Horizontal);

	// INSERT PANEL:
	QSizePolicy sizePolicy(QSizePolicy::Minimum, QSizePolicy::Preferred);
	sizePolicy.setHorizontalStretch(0);
	sizePolicy.setVerticalStretch(0);
	sizePolicy.setHeightForWidth(insert_panel.sizePolicy().hasHeightForWidth());
	insert_panel.setSizePolicy(sizePolicy);
	insert_panel.setMinimumSize(QSize(150, 0));

	QPalette palette_dark;

	palette_dark.setColor(QPalette::WindowText, QColor(255, 255, 255, 255));
	palette_dark.setColor(QPalette::Text, QColor(255, 255, 255, 255));
	palette_dark.setColor(QPalette::Base, QColor(80, 80, 80, 255));
	palette_dark.setColor(QPalette::Window, QColor(80, 80, 80, 255));
	palette_dark.setColor(QPalette::AlternateBase, QColor(0, 0, 0, 255));

	insert_panel.setPalette(palette_dark);
	insert_panel.setAutoFillBackground(true);

	//insert_object_list.setStyleSheet( "QTreeView QLineEdit {background-color:#555;}");

	insert_panel_layout.setSpacing(0);
	insert_panel_layout.setContentsMargins(0, 6, 0, 0);

	// INSERT PANEL TITLE:
	QFont font;
	font.setFamily(QString::fromUtf8("Noto Sans"));
	font.setBold(true);
	font.setWeight(75);
	insert_panel_title.setFont(font);
	insert_panel_title.setAlignment(Qt::AlignCenter);

	insert_panel_layout.addWidget(&insert_panel_title);

	// INSERT PANEL OBJECT_VIEWER:
	object_viewer.setMinimumHeight(80);
	object_viewer.set_frame_size(0, 0, 0);
	insert_panel_layout.addWidget(&object_viewer);

	// INSERT OBJECT LISTBOX:
	insert_object_list.setFrameShape(QFrame::NoFrame);
	insert_object_list.setFrameShadow(QFrame::Plain);
	insert_object_list.setHeaderHidden(true);

	insert_panel_layout.addWidget(&insert_object_list);

	// "INSERT OBJECT DETAILS":
	insert_object_details.setTitle(" Details");
	insert_object_details.setLayout(&insert_object_details_layout);
	object_details_label.setWordWrap(true);
	object_details_label.setText("This is a description. A kind of long description. \n It even contains a newline. Holy crap this is a very long sentence. I hope it will word-wrap properly.");
	insert_object_details_layout.addWidget(&object_details_label);

	insert_panel_layout.addWidget(&insert_object_details);

	main_splitter.addWidget(&insert_panel);

	// LEVEL EDITOR WIDGET:
	editor_panel_layout.setSpacing(0);
	editor_panel_layout.setContentsMargins(0, 0, 0, 0);

	editor_panel_v_scrollbar.setMaximum(2048);
	editor_panel_v_scrollbar.setOrientation(Qt::Vertical);

	editor_panel_h_scrollbar.setMaximum(2048);
	editor_panel_h_scrollbar.setOrientation(Qt::Horizontal);

	editor_panel_layout.addWidget(&editor_panel_v_scrollbar, 0, 1, 1, 1);
	editor_panel_layout.addWidget(&editor_panel_h_scrollbar, 1, 0, 1, 1);

	QSizePolicy sizePolicy2(QSizePolicy::Minimum, QSizePolicy::Preferred);
	sizePolicy2.setHorizontalStretch(1);
	sizePolicy2.setVerticalStretch(1);
	sizePolicy2.setHeightForWidth(insert_panel.sizePolicy().hasHeightForWidth());
	editor_panel.setSizePolicy(sizePolicy2);

	editor_panel_layout.addWidget(&level_editor, 0, 0, 1, 1);

	editor_panel.setLayout(&editor_panel_layout);

	main_splitter.addWidget(&editor_panel);

	central_layout.addWidget(&main_splitter,0,0);

	this->setLayout(&central_layout);

	insert_panel_title.setText(QApplication::translate("window_main", "Insert", 0));
}

void level_editor_widget::show_toolbar_items(QToolBar & toolbar, QWidget *parent)
{	
	is_active = true;

	// Delete and remake the widgets in order to solve a Qt bug:
	int selected_level = level_selector->value();
	delete level_selector;
	level_selector = new HexSpinBox(false, parent);
	level_selector->setRange(0x00, 0x1FF);
	level_selector->setValue(selected_level);

	int selected_zoom = zoom_slider->value();
	delete zoom_slider;
	zoom_slider = new QSlider(Qt::Horizontal, parent);
	zoom_slider->setRange(0, 4);
	zoom_slider->setValue(selected_zoom);
	zoom_slider->setMaximumWidth(100);

	level_selector->connect(level_selector, SIGNAL(valueChanged(int)), this, SLOT(select_level(int)));
	zoom_slider->connect(zoom_slider, SIGNAL(valueChanged(int)), this, SLOT(on_zoom_slider_slider_moved(int)));

	if(action_move_up != NULL)
		delete action_move_up;

	if(action_move_down != NULL)
		delete action_move_down;

	if(action_move_to_top != NULL)
		delete action_move_to_top;

	if(action_move_to_bottom != NULL)
		delete action_move_to_bottom;

	action_move_up = new QAction(QIcon::fromTheme("go-up"), "Bring Forward", parent);
	action_move_down = new QAction(QIcon::fromTheme("go-down"), "Send Backward", parent);
	action_move_to_top = new QAction(QIcon::fromTheme("go-top"), "Bring to Front", parent);
	action_move_to_bottom = new QAction(QIcon::fromTheme("go-bottom"), "Send to Back", parent);

	action_move_up->connect(action_move_up, SIGNAL(triggered()), this, SLOT(on_action_move_up_triggered()));
	action_move_down->connect(action_move_down, SIGNAL(triggered()), this, SLOT(on_action_move_down_triggered()));
	action_move_to_top->connect(action_move_to_top, SIGNAL(triggered()), this, SLOT(on_action_move_to_top_triggered()));
	action_move_to_bottom->connect(action_move_to_bottom, SIGNAL(triggered()), this, SLOT(on_action_move_to_bottom_triggered()));

	if(action_edit_layer_1 != NULL)
		delete action_edit_layer_1;

	if(action_edit_layer_2 != NULL)
		delete action_edit_layer_2;

	if(action_edit_sprites != NULL)
		delete action_edit_sprites;

	action_edit_layer_1 = new QAction(QIcon::fromTheme("layer-1"), "Layer 1", parent);
	action_edit_layer_2 = new QAction(QIcon::fromTheme("layer-2"), "Layer 2", parent);
	action_edit_sprites = new QAction(QIcon::fromTheme("sprites"), "Sprites", parent);

	action_edit_layer_1->setCheckable(true);
	action_edit_layer_2->setCheckable(true);
	action_edit_sprites->setCheckable(true);
	action_edit_layer_1->setChecked(true);

	action_edit_layer_1->connect(action_edit_layer_1, SIGNAL(triggered()), this, SLOT(on_action_edit_layer_1_triggered()));
	action_edit_layer_2->connect(action_edit_layer_2, SIGNAL(triggered()), this, SLOT(on_action_edit_layer_2_triggered()));
	action_edit_sprites->connect(action_edit_sprites, SIGNAL(triggered()), this, SLOT(on_action_edit_sprites_triggered()));

	// Spacer Widget:
	if(spacer_widget != NULL)
		delete spacer_widget;

	spacer_widget = new QWidget(this);
	spacer_widget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
	spacer_widget->setVisible(true);

	// Fill the toolbar:
	toolbar.clear();
	toolbar.addWidget(level_selector);


	toolbar.addSeparator();

	toolbar.addAction(action_edit_layer_1);
	toolbar.addAction(action_edit_layer_2);
	toolbar.addAction(action_edit_sprites);

	toolbar.addSeparator();

	toolbar.addAction(action_move_to_top);
	toolbar.addAction(action_move_up);
	toolbar.addAction(action_move_down);
	toolbar.addAction(action_move_to_bottom);

	toolbar.addWidget(spacer_widget);			// Everything after this will be on the right side.

	toolbar.addWidget(zoom_slider);

	select_level(selected_level);
}

void level_editor_widget::stop_animation()
{
	level_editor.stop_animation();
	object_viewer.stop_animation();

	is_active = false;
}

void level_editor_widget::start_animation()
{
	if(opened_rom.is_open() == false)
	{
		emit emit_message(message_type::warning, "No ROM file loaded yet!");
		return;
	}

	level_editor.start_animation(opened_rom, opened_level.header.tile_set);

	is_active = true;
}

void level_editor_widget::on_rom_load()
{
	// Update all object view widgets, because the rom has reloaded:
	select_level(level_selector->value());
	on_insert_object_list_selection_changed(insert_object_list.selectionModel()->currentIndex(), insert_object_list.selectionModel()->currentIndex());
}

bool level_editor_widget::analyze_object_types()
{
	if(current_layer == sprites)
	{
		if(object_types_items.rowCount() != 0x100)
		{
			object_types_items.clear();

			// Fill the object types list (in the 'Insert' menu and 'Object Settings' setting):
			int a;
			for (a=0; a<0x100; a++) {
				// This can be 'new', since 'delete' will be called on removeRow anyway. This is not a memory leak.
				object_types_items.appendRow(new QStandardItem(QString("%1").arg(a, 2, 16, QLatin1Char( '0' )).toUpper() + ": Object"));
			}
		}

		// Fill the object types list (in the 'Insert' menu and 'Object Settings' setting):
		for (int a=0; a<0x100; a++) {
			object_types_items.item(a)->removeRows(0, object_types_items.item(a)->rowCount());
			object_types_items.item(a)->setText(QString("%1").arg(a, 2, 16, QLatin1Char( '0' )).toUpper() + ": " + (char *)opened_level.toolbox_sprites.items[a].name.getBuffer());

			//for (int b=0; b<opened_level.toolbox.items[a].number_of_subtypes; b++) {
			//	// This can be 'new', since 'delete' will be called on removeRow anyway. This is not a memory leak.
			//	object_types_items.item(a)->appendRow(new QStandardItem(QString("%1").arg(b, 2, 16, QLatin1Char( '0' )).toUpper() + ": SubObject"));
			//}

		}

		return true;
	} else
	{
		if(object_types_items.rowCount() != 0x40)
		{
			object_types_items.clear();

			// Fill the object types list (in the 'Insert' menu and 'Object Settings' setting):
			int a;
			for (a=0; a<0x40; a++) {
				// This can be 'new', since 'delete' will be called on removeRow anyway. This is not a memory leak.
				object_types_items.appendRow(new QStandardItem(QString("%1").arg(a, 2, 16, QLatin1Char( '0' )).toUpper() + ": Object"));
			}
		}

		// Fill the object types list (in the 'Insert' menu and 'Object Settings' setting):
		for (int a=0; a<0x40; a++) {
			object_types_items.item(a)->removeRows(0, object_types_items.item(a)->rowCount());
			object_types_items.item(a)->setText(QString("%1").arg(a, 2, 16, QLatin1Char( '0' )).toUpper() + ": " + (char *)opened_level.toolbox.items[a].name.getBuffer());

			for (int b=0; b<opened_level.toolbox.items[a].number_of_subtypes; b++) {
				// This can be 'new', since 'delete' will be called on removeRow anyway. This is not a memory leak.
				object_types_items.item(a)->appendRow(new QStandardItem(QString("%1").arg(b, 2, 16, QLatin1Char( '0' )).toUpper() + ": SubObject"));
			}

		}
	}

	return true;
}

void level_editor_widget::select_level(int index)
{
	if(opened_rom.is_open() == false)
	{
		emit emit_message(message_type::warning, "No ROM file loaded yet!");
		return;
	}

	deselect_object();
	insert_object_list.selectionModel()->select(QModelIndex(), QItemSelectionModel::Clear);
	object_viewer.set_frame_size(64, 64, 1);
	clear_object_viewer();

	// Load the level:
	int error_count = opened_level.load_level(index, opened_rom);

	// Handle error messages:
	if(error_count <= -1)
		emit emit_message(message_type::error, "Level index out of bounds!");
	else if(error_count != 0)
		emit emit_message(message_type::warning, "The level contains " + QString("%1").arg(error_count) + " faulty objects.");

	// Set the proper size of the level display widget:
	if(opened_level.header.is_vertical_level == true)
		level_editor.set_frame_size(64, 1024, 2);
	else
		level_editor.set_frame_size(1024, 54, 2);

	calculate_scrollbar_size();

	// Load the level graphics and display it:
	load_vanilla_map16(opened_map16, opened_rom, opened_level.header.tile_set);
	emulation_error error1 = display_level_map(opened_map16, opened_level.level_map[LEVEL_LAYER_1], level_editor.get_tilemap_texture(LAYER_1));
	emulation_error error2 = display_level_map(opened_map16, opened_level.level_map[LEVEL_LAYER_2], level_editor.get_tilemap_texture(LAYER_2));

	if((error1 != emulation_error::emulator_success || error2 != emulation_error::emulator_success) && error_count == 0)
		emit emit_message(message_type::warning, "The level contains map16 tiles which do not exist!");

	//prepare_display_level_sprites(opened_level.toolbox_sprites);
	display_level_sprites(opened_level.sprite_list, level_editor.sprite_list, opened_level.toolbox_sprites);

	load_vanilla_gfx(opened_rom, level_editor.m_terrain_gfx_texture, level_editor.m_sprite_gfx_texture, opened_level.header);
	if(load_vanilla_palette(opened_rom, level_editor.m_palette_texture, opened_level.header) != emulation_error::emulator_success)
		emit emit_message(message_type::error, "Could not load palette; the palette routine in the ROM might be corrupted.");

	level_editor.update();

	analyze_object_types();

	// Disable or enable layer 2 editing, depending on the level type:
	if(action_edit_layer_2 != NULL)
	{
		if(opened_level.header.is_layer2_level == false)
		{
			if(action_edit_layer_2->isChecked() == true)
				on_action_edit_layer_1_triggered();
			action_edit_layer_2->setEnabled(false);
		} else {
			action_edit_layer_2->setEnabled(true);
		}
	}

	//int a;

	//ui->object_list->setCurrentIndex(QModelIndex());
	//for (a=object_list_items.rowCount(); a>=0; a--) {
	//	object_list_items.removeRow(a);
	//}

	//for (a=0; a<opened_level[LEVEL_LAYER_1].object_list.getLength(); a++) {
		// The next line can contain 'new', since 'delete' will be called on removeRow anyway (QT functionality). This is not a memory leak.
		//object_list_items.appendRow(new QStandardItem(QString("%1").arg(a, 2, 16, QLatin1Char( '0' )).toUpper() + ": " + QString("%1").arg(opened_level[LEVEL_LAYER_1].object_list.getObject(a)->object_number, 2, 16, QLatin1Char( '0' )).toUpper()));

	//}

	// Populate the Insert Object treeview:



	//on_insert_object_list_selectionchanged(ui->insert_object_list->selectionModel()->currentIndex(), ui->insert_object_list->selectionModel()->currentIndex());


	// TODO: make a menu item making animations toggleable:
	if(is_active == true)
		level_editor.start_animation(opened_rom, opened_level.header.tile_set);
}

bool level_editor_widget::select_object(unsigned int object_index)
{
	if(current_layer == sprites)
	{
		qWarning() << "WARNING: Selecting object on non-existing layer!";
		return false;
	}

	if(object_index >= opened_level.object_list[current_layer].get_length())
	{
		deselect_object();
		qWarning() << "No valid object selected!";
		return false;
	}

	// Make sure there won't be any redundant emulations:
	emulate_lock = true;

	selected_object = object_index;



	//QModelIndex index_of_selected_object = ui->object_list->model()->index(selected_object, 0);

	//ui->object_list->setCurrentIndex(index_of_selected_object);

	//ui->object_settings_x->setValue(opened_level[current_layer].object_list.getObject(selected_object)->position_X);					// Set the 'X-location' setting to the proper value.
	//ui->object_settings_y->setValue(opened_level[current_layer].object_list.getObject(selected_object)->position_Y);					// Set the 'Y-location' setting to the proper value.
	//ui->object_settings_object->setCurrentIndex(opened_level[current_layer].object_list.getObject(selected_object)->object_number);	// Set the 'Object Type' setting to the proper value.

	unsigned int current_object_number = opened_level.object_list[current_layer].get_item(selected_object).object_number;
	unsigned int current_object_type = opened_level.object_list[current_layer].get_item(selected_object).get_type();

	if(current_object_number < 0x40)
	{
		QModelIndex selected_node;

		if(opened_level.toolbox.items[current_object_number].number_of_subtypes > 0)
			selected_node = insert_object_list.model()->index(current_object_type, 0, insert_object_list.model()->index(current_object_number, 0, QModelIndex()));
		else
			selected_node = insert_object_list.model()->index(current_object_number, 0, QModelIndex());

		insert_object_list.selectionModel()->select(selected_node, QItemSelectionModel::SelectCurrent);
		insert_object_list.scrollTo(selected_node);
		display_object_viewer(selected_node);
	}

	// Depending on the data type of the object, show or hide certain settings widgets:
	//update_visible_settings(current_object_number, opened_level[current_layer].toolbox);

	// Set the proper value for each of the settings boxes:
	//ui->object_settings_height->setValue(opened_level[current_layer].object_list.getObject(selected_object)->get_height());
	//ui->object_settings_width->setValue(opened_level[current_layer].object_list.getObject(selected_object)->get_width());
	//ui->object_settings_type->setCurrentIndex(opened_level[current_layer].object_list.getObject(selected_object)->get_type());

	// Show the selection and the resize cursors in the editor widget:
	level_editor.set_selection_cursor_position(opened_level.object_list[current_layer].get_item(selected_object).position_X * 2, opened_level.object_list[current_layer].get_item(selected_object).position_Y * 2);
	level_editor.set_resize_cursor_position((opened_level.object_list[current_layer].get_item(selected_object).position_X + opened_level.object_list[current_layer].get_item(selected_object).get_width()) * 2 + 1,
											(opened_level.object_list[current_layer].get_item(selected_object).position_Y + opened_level.object_list[current_layer].get_item(selected_object).get_height()) * 2 + 1);

	// Enable emulations again:
	emulate_lock = false;

	return true;
}

bool level_editor_widget::deselect_object()
{
	// Make sure there won't be any redundant emulations:
	emulate_lock = true;

	selected_object = -1;

	level_editor.set_selection_cursor_position(-1, -1);
	level_editor.set_resize_cursor_position(-1, -1);

	//ui->object_list->setCurrentIndex(QModelIndex());

	// Enable emulations again:
	emulate_lock = false;

	return true;
}

// Zoom and scroll:
void level_editor_widget::on_v_scrollbar_slider_moved(int position)
{
	level_editor.moveScrollY(position);
}

void level_editor_widget::on_h_scrollbar_slider_moved(int position)
{
	level_editor.moveScrollX(position);
}

void level_editor_widget::on_zoom_slider_slider_moved(int position)
{
	level_editor.setZoom(pow(2.0, (float)position-2.0));

	calculate_scrollbar_size();
}

void level_editor_widget::on_main_splitter_moved(int pos, int index)
{
	object_viewer.moveScrollX(-object_viewer.get_width() / 2 / 4 + 32);
	object_viewer.update();
}

void level_editor_widget::resizeEvent(QResizeEvent * event)
{
	calculate_scrollbar_size();
}

void level_editor_widget::calculate_scrollbar_size()
{
	// Recalculate scrollbar size:
	if(opened_level.header.is_vertical_level == true)
	{
		editor_panel_h_scrollbar.setMaximum((64 - level_editor.get_visible_tiles_width()) * 2 + 2);
		editor_panel_v_scrollbar.setMaximum((1024 - level_editor.get_visible_tiles_height()) * 2 + 2);
	}
	else
	{
		editor_panel_h_scrollbar.setMaximum((1024 - level_editor.get_visible_tiles_width()) * 2 + 2);
		editor_panel_v_scrollbar.setMaximum((54 - level_editor.get_visible_tiles_height()) * 2 + 2);
	}

	editor_panel_h_scrollbar.setMinimum(0);
	editor_panel_v_scrollbar.setMinimum(0);

	if(editor_panel_h_scrollbar.maximum() <= 0) {
		editor_panel_h_scrollbar.setValue(0);
		editor_panel_h_scrollbar.hide();
	} else {
		editor_panel_h_scrollbar.show();
	}

	if(editor_panel_v_scrollbar.maximum() <= 0) {
		editor_panel_v_scrollbar.setValue(0);
		editor_panel_v_scrollbar.hide();
	} else {
		editor_panel_v_scrollbar.show();
	}
}

// Select and drag objects:
void level_editor_widget::on_level_editor_left_mouse_pressed(int x, int y)
{
	if(current_layer == sprites)
	{
		qWarning() << "TODO: EDITING SPRITES!";
		return;
	}

	has_dragged = false;

	if((x < 0) || (x >= opened_level.level_map[current_layer].getMapSizeX()) || (y < 0) || (y >= opened_level.level_map[current_layer].getMapSizeY()))
		return;

	unsigned int owner = opened_level.level_map[current_layer].getMapDataOwner(x, y);

	if(select_object(owner - 1) == true)
	{
		// These variables make sure that the dragging of the objects go smoothly:
		relative_selection_x = x - opened_level.object_list[current_layer].get_item(selected_object).position_X;
		relative_selection_y = y - opened_level.object_list[current_layer].get_item(selected_object).position_Y;
	}
}

void level_editor_widget::on_level_editor_left_mouse_dragged(int x, int y)
{
	if(current_layer == sprites)
	{
		qWarning() << "TODO: EDITING SPRITES!";
		return;
	}

	has_dragged = true;

	if(selected_object >= opened_level.object_list[current_layer].get_length())
		return;

	// Make sure that the object does not get dragged beyond the bounds:
	if(x - relative_selection_x < 0)
		x = relative_selection_x;
	else if(x - relative_selection_x >= opened_level.level_map[current_layer].getMapSizeX())
		x = opened_level.level_map[current_layer].getMapSizeX() - 1 + relative_selection_x;

	if(y - relative_selection_y < 0)
		y = relative_selection_y;
	else if(y - relative_selection_y >= opened_level.level_map[current_layer].getMapSizeY())
		y = opened_level.level_map[current_layer].getMapSizeY() - 1 + relative_selection_y;

	level_editor.makeCurrent();

	// Make sure there won't be any redundant emulations:
	emulate_lock = true;

	//ui->object_settings_x->setValue(x - relative_selection_x);					// Set the 'X-location' setting to the proper value.
	//ui->object_settings_y->setValue(y - relative_selection_y);					// Set the 'Y-location' setting to the proper value.

	opened_level.move_object_y(current_layer, selected_object, y - relative_selection_y);
	opened_level.move_object_x(current_layer, selected_object, x - relative_selection_x);

	// Show the selection and resize cursors move in the editor widget:
	level_editor.set_selection_cursor_position(opened_level.object_list[current_layer].get_item(selected_object).position_X * 2, opened_level.object_list[current_layer].get_item(selected_object).position_Y * 2);
	level_editor.set_resize_cursor_position((opened_level.object_list[current_layer].get_item(selected_object).position_X + opened_level.object_list[current_layer].get_item(selected_object).get_width()) * 2 + 1,
											(opened_level.object_list[current_layer].get_item(selected_object).position_Y + opened_level.object_list[current_layer].get_item(selected_object).get_height()) * 2 + 1);

	// Emulate and update the changes (only update the visible region):
	int visible_x_min = level_editor.get_visible_tiles_x() / 2;
	int visible_y_min = level_editor.get_visible_tiles_y() / 2;
	int visible_x_max = (level_editor.get_visible_tiles_x() + level_editor.get_visible_tiles_width()) / 2;
	int visible_y_max = (level_editor.get_visible_tiles_y() + level_editor.get_visible_tiles_height()) / 2;

	opened_level.update_level(opened_rom, current_layer);
	display_level_map(opened_map16, opened_level.level_map[current_layer], level_editor.get_tilemap_texture(LAYER_1), visible_x_min, visible_y_min, visible_x_max, visible_y_max);	// Update only the visible region to reduce lag.
	//level_editor.updateGL();

	level_editor.set_tilemap_opacity(LAYER_2, 0.5f);

	// Enable emulations again:
	emulate_lock = false;
}

void level_editor_widget::on_level_editor_left_mouse_released(int x, int y)
{
	// Set focus to "this", so that it can receive keyboard events:
	setFocus();

	if(current_layer == sprites)
	{
		qWarning() << "TODO: EDITING SPRITES!";
		return;
	}

	// Make sure there won't be any redundant emulations:
	emulate_lock = true;

	level_editor.set_tilemap_opacity(LAYER_2, 1.0f);

	if(has_dragged == false)
	{
		if((x < 0) || (x >= opened_level.level_map[current_layer].getMapSizeX()) || (y < 0) || (y >= opened_level.level_map[current_layer].getMapSizeY()))
			return;

		unsigned int owner = opened_level.level_map[current_layer].getMapDataOwner(x, y);

		select_object(owner - 1);
	} else {
		level_editor.makeCurrent();
		// Update the entire level:
		if(current_layer == layer_2)
		{
			display_level_map(opened_map16, opened_level.level_map[LEVEL_LAYER_1], level_editor.get_tilemap_texture(LAYER_2));
			display_level_map(opened_map16, opened_level.level_map[LEVEL_LAYER_2], level_editor.get_tilemap_texture(LAYER_1));
		} else
		{
			display_level_map(opened_map16, opened_level.level_map[LEVEL_LAYER_1], level_editor.get_tilemap_texture(LAYER_1));
			display_level_map(opened_map16, opened_level.level_map[LEVEL_LAYER_2], level_editor.get_tilemap_texture(LAYER_2));
		}

		level_editor.updateGL();
	}

	// Enable emulations again:
	emulate_lock = false;


}

void level_editor_widget::on_level_editor_right_mouse_pressed(int x, int y)
{
	if(current_layer == sprites)
	{
		qWarning() << "TODO: EDITING SPRITES!";
		return;
	}

	if((x < 0) || (x >= opened_level.level_map[current_layer].getMapSizeX()) || (y < 0) || (y >= opened_level.level_map[current_layer].getMapSizeY()))
		return;

	level_editor.makeCurrent();

	level_object temp_object;

	if(selected_object >= opened_level.object_list[current_layer].get_length())
	{
		if(insert_object_list.currentIndex().row() < 0)
			return;

		// No object selected, so insert one from the "insert" window:

		// If the selected index is a sub, then this index holds the object subtype and the parent holds the object type:
		bool is_sub = insert_object_list.currentIndex().parent().isValid();

		if(is_sub==true)
		{
			temp_object.object_number = insert_object_list.currentIndex().parent().row();
			temp_object.data_format = opened_level.toolbox.items[temp_object.object_number].format;

			temp_object.set_type(insert_object_list.currentIndex().row());
		}
		else
		{
			temp_object.object_number = insert_object_list.currentIndex().row();
			temp_object.data_format = opened_level.toolbox.items[temp_object.object_number].format;

			temp_object.set_type(0);
		}

		temp_object.set_height(2);
		temp_object.set_width(2);


	} else {
		// Object selected, so copy it:
		temp_object = opened_level.object_list[current_layer].get_item(selected_object);
	}

	// TODO: remove the next hard-coded disability:

	// If the object is a Screen Exit or a Screen Move object, do not add it:
	if((temp_object.object_number == 0x00) && (temp_object.settings_byte_1 < 0x10 ))
	{
		emit emit_message(message_type::error, "Cannot manually add a \"Screen Exit\", a \"Jump To Screen\" object or a reserved object.");
		return;
	}



	opened_level.object_list[current_layer].add_item(temp_object);

	// Calculate the index for the list (this one is purposefully different from the previous time the index was calculated, two lines above):
	unsigned int object_index = opened_level.object_list[current_layer].get_length() - 1;

	// Move the object to the appropriate position:
	opened_level.move_object_x(current_layer, object_index, x);
	opened_level.move_object_y(current_layer, object_index, y);

	// Add the new object to the list in the UI:
	//object_list_items.appendRow(new QStandardItem(QString("%1").arg(object_index, 2, 16, QLatin1Char( '0' )).toUpper() + ": " + QString("%1").arg(opened_level.object_list[LEVEL_LAYER_1].get_item(object_index).object_number, 2, 16, QLatin1Char( '0' )).toUpper()));

	select_object(object_index);

	// Emulate and update the changes:
	opened_level.update_level(opened_rom, current_layer);
	display_level_map(opened_map16, opened_level.level_map[current_layer], level_editor.get_tilemap_texture(LAYER_1));
	level_editor.update();
}

void level_editor_widget::on_level_editor_right_mouse_dragged(int x, int y)
{

}

void level_editor_widget::on_level_editor_top_cursor_dragged(int x, int y)
{
	if(current_layer == sprites)
	{
		qWarning() << "TODO: EDITING SPRITES!";
		return;
	}

	// Both resize and move the base of the selected object:

	if(selected_object >= opened_level.object_list[current_layer].get_length())
		return;

	level_editor.makeCurrent();

	// Make sure there won't be any redundant emulations:
	emulate_lock = true;

	// Prevent editing outside of the bounds of the level:
	if(x < 0)
		x = 0;
	else if(x >= opened_level.level_map[current_layer].getMapSizeX())
		x = opened_level.level_map[current_layer].getMapSizeX() - 1;

	if(y < 0)
		y = 0;
	else if(y >= opened_level.level_map[current_layer].getMapSizeY())
		y = opened_level.level_map[current_layer].getMapSizeY() - 1;

	int object_x_shift = opened_level.object_list[current_layer].get_item(selected_object).position_X - x;
	int object_y_shift = opened_level.object_list[current_layer].get_item(selected_object).position_Y - y;

	//ui->object_settings_height->setValue(opened_level[current_layer].object_list.getObject(selected_object)->get_height() + object_y_shift);
	//ui->object_settings_width->setValue(opened_level[current_layer].object_list.getObject(selected_object)->get_width() + object_x_shift);

	level_object temp_object = opened_level.object_list[current_layer].get_item(selected_object);

	temp_object.set_width(opened_level.object_list[current_layer].get_item(selected_object).get_width() + object_x_shift);
	temp_object.set_height(opened_level.object_list[current_layer].get_item(selected_object).get_height() + object_y_shift);

	opened_level.object_list[current_layer].set_item(selected_object, temp_object);

	//ui->object_settings_x->setValue(x);					// Set the 'X-location' setting to the proper value.
	//ui->object_settings_y->setValue(y);					// Set the 'Y-location' setting to the proper value.

	opened_level.move_object_y(current_layer, selected_object, y);
	opened_level.move_object_x(current_layer, selected_object, x);

	// Show the selection and resize cursors move in the editor widget:
	level_editor.set_selection_cursor_position(opened_level.object_list[current_layer].get_item(selected_object).position_X * 2, opened_level.object_list[current_layer].get_item(selected_object).position_Y * 2);
	level_editor.set_resize_cursor_position((opened_level.object_list[current_layer].get_item(selected_object).position_X + opened_level.object_list[current_layer].get_item(selected_object).get_width()) * 2 + 1,
											(opened_level.object_list[current_layer].get_item(selected_object).position_Y + opened_level.object_list[current_layer].get_item(selected_object).get_height()) * 2 + 1);

	// Emulate and update the changes (only update the visible region):
	int visible_x_min = level_editor.get_visible_tiles_x() / 2;
	int visible_y_min = level_editor.get_visible_tiles_y() / 2;
	int visible_x_max = (level_editor.get_visible_tiles_x() + level_editor.get_visible_tiles_width()) / 2;
	int visible_y_max = (level_editor.get_visible_tiles_y() + level_editor.get_visible_tiles_height()) / 2;

	opened_level.update_level(opened_rom, current_layer);
	display_level_map(opened_map16, opened_level.level_map[current_layer], level_editor.get_tilemap_texture(LAYER_1), visible_x_min, visible_y_min, visible_x_max, visible_y_max);	// Update only the visible region to reduce lag.
	//level_editor.update();

	level_editor.set_tilemap_opacity(LAYER_2, 0.5f);

	// Enable emulations again:
	emulate_lock = false;
}

void level_editor_widget::on_level_editor_bottom_cursor_dragged(int x, int y)
{
	if(current_layer == sprites)
	{
		qWarning() << "TODO: EDITING SPRITES!";
		return;
	}

	// Only resize the selected object:

	if(selected_object >= opened_level.object_list[current_layer].get_length())
		return;

	level_editor.makeCurrent();

	// Make sure there won't be any redundant emulations:
	emulate_lock = true;

	//ui->object_settings_height->setValue(y - opened_level[current_layer].object_list.getObject(selected_object)->position_Y);
	//ui->object_settings_width->setValue(x - opened_level[current_layer].object_list.getObject(selected_object)->position_X);

	//unsigned int width = opened_level[current_layer].object_list.get_object(selected_object)->get_width();
	//unsigned int height = opened_level[current_layer].object_list.get_object(selected_object)->get_height();

	level_object temp_object = opened_level.object_list[current_layer].get_item(selected_object);

	temp_object.set_width(x - opened_level.object_list[current_layer].get_item(selected_object).position_X);
	temp_object.set_height(y - opened_level.object_list[current_layer].get_item(selected_object).position_Y);

	opened_level.object_list[current_layer].set_item(selected_object, temp_object);

	// Prevent value overflows (which look ugly):
	//if(x - opened_level[current_layer].object_list.get_object(selected_object)->position_X != opened_level[current_layer].object_list.get_object(selected_object)->get_width())
	//	opened_level[current_layer].object_list.get_object(selected_object)->set_width(width);

	//if(y - opened_level[current_layer].object_list.get_object(selected_object)->position_Y != opened_level[current_layer].object_list.get_object(selected_object)->get_height())
	//	opened_level[current_layer].object_list.get_object(selected_object)->set_height(height);

	// Show the selection and resize cursors move in the editor widget:
	level_editor.set_selection_cursor_position(opened_level.object_list[current_layer].get_item(selected_object).position_X * 2, opened_level.object_list[current_layer].get_item(selected_object).position_Y * 2);
	level_editor.set_resize_cursor_position((opened_level.object_list[current_layer].get_item(selected_object).position_X + opened_level.object_list[current_layer].get_item(selected_object).get_width()) * 2 + 1,
											(opened_level.object_list[current_layer].get_item(selected_object).position_Y + opened_level.object_list[current_layer].get_item(selected_object).get_height()) * 2 + 1);

	// Emulate and update the changes (only update the visible region):
	int visible_x_min = level_editor.get_visible_tiles_x() / 2;
	int visible_y_min = level_editor.get_visible_tiles_y() / 2;
	int visible_x_max = (level_editor.get_visible_tiles_x() + level_editor.get_visible_tiles_width()) / 2;
	int visible_y_max = (level_editor.get_visible_tiles_y() + level_editor.get_visible_tiles_height()) / 2;

	opened_level.update_level(opened_rom, current_layer);
	display_level_map(opened_map16, opened_level.level_map[current_layer], level_editor.get_tilemap_texture(LAYER_1), visible_x_min, visible_y_min, visible_x_max, visible_y_max);	// Update only the visible region to reduce lag.
	//level_editor.update();

	level_editor.set_tilemap_opacity(LAYER_2, 0.5f);

	// Enable emulations again:
	emulate_lock = false;
}

void level_editor_widget::on_level_editor_cursor_released(int x, int y)
{
	setFocus();

	if(current_layer == sprites)
	{
		qWarning() << "TODO: EDITING SPRITES!";
		return;
	}

	level_editor.makeCurrent();

	// Update the entire level:
	if(current_layer == layer_2)
	{
		display_level_map(opened_map16, opened_level.level_map[LEVEL_LAYER_1], level_editor.get_tilemap_texture(LAYER_2));
		display_level_map(opened_map16, opened_level.level_map[LEVEL_LAYER_2], level_editor.get_tilemap_texture(LAYER_1));
	} else
	{
		display_level_map(opened_map16, opened_level.level_map[LEVEL_LAYER_1], level_editor.get_tilemap_texture(LAYER_1));
		display_level_map(opened_map16, opened_level.level_map[LEVEL_LAYER_2], level_editor.get_tilemap_texture(LAYER_2));
	}

	level_editor.set_tilemap_opacity(LAYER_2, 1.0f);

	level_editor.updateGL();
}

void level_editor_widget::on_level_editor_right_mouse_released(int x, int y)
{
	setFocus();
}

void level_editor_widget::on_action_move_up_triggered()
{
	if(current_layer == sprites)
	{
		qWarning() << "TODO: EDITING SPRITES!";
		return;
	}

	if(selected_object >= opened_level.object_list[current_layer].get_length())
		return;

	if (selected_object >= opened_level.object_list[current_layer].get_length() - 1)
		return;

	// Swap objects:
	level_object object_selected = opened_level.object_list[current_layer].get_item(selected_object);
	level_object object_other = opened_level.object_list[current_layer].get_item(selected_object + 1);

	level_editor.makeCurrent();

	opened_level.object_list[current_layer].set_item(selected_object, object_other);
	opened_level.object_list[current_layer].set_item(selected_object + 1, object_selected);

	selected_object++;

	opened_level.update_level(opened_rom, current_layer);
	display_level_map(opened_map16, opened_level.level_map[current_layer], level_editor.get_tilemap_texture(LAYER_1));

	level_editor.update();

	select_object(selected_object);
}

void level_editor_widget::on_action_move_down_triggered()
{
	if(current_layer == sprites)
	{
		qWarning() << "TODO: EDITING SPRITES!";
		return;
	}

	if(selected_object >= opened_level.object_list[current_layer].get_length())
		return;

	if (selected_object == 0)
		return;

	// Swap objects:
	level_object object_selected = opened_level.object_list[current_layer].get_item(selected_object);
	level_object object_other = opened_level.object_list[current_layer].get_item(selected_object - 1);

	level_editor.makeCurrent();

	opened_level.object_list[current_layer].set_item(selected_object, object_other);
	opened_level.object_list[current_layer].set_item(selected_object - 1, object_selected);

	selected_object--;

	opened_level.update_level(opened_rom, current_layer);
	display_level_map(opened_map16, opened_level.level_map[current_layer], level_editor.get_tilemap_texture(LAYER_1));

	level_editor.update();

	select_object(selected_object);
}

void level_editor_widget::on_action_move_to_top_triggered()
{
	if(current_layer == sprites)
	{
		qWarning() << "TODO: EDITING SPRITES!";
		return;
	}

	if(selected_object >= opened_level.object_list[current_layer].get_length())
		return;

	level_object object = opened_level.object_list[current_layer].get_item(selected_object);

	level_editor.makeCurrent();

	opened_level.object_list[current_layer].remove_item(selected_object);
	opened_level.object_list[current_layer].add_item(object);

	selected_object = opened_level.object_list[current_layer].get_length() - 1;

	opened_level.update_level(opened_rom, current_layer);
	display_level_map(opened_map16, opened_level.level_map[current_layer], level_editor.get_tilemap_texture(LAYER_1));

	level_editor.update();

	select_object(selected_object);

}

void level_editor_widget::on_action_move_to_bottom_triggered()
{
	if(current_layer == sprites)
	{
		qWarning() << "TODO: EDITING SPRITES!";
		return;
	}

	if(selected_object >= opened_level.object_list[current_layer].get_length())
		return;

	level_object object = opened_level.object_list[current_layer].get_item(selected_object);

	level_editor.makeCurrent();

	opened_level.object_list[current_layer].remove_item(selected_object);
	opened_level.object_list[current_layer].insert_item(0, object);

	selected_object = 0;

	opened_level.update_level(opened_rom, current_layer);
	display_level_map(opened_map16, opened_level.level_map[current_layer], level_editor.get_tilemap_texture(LAYER_1));

	level_editor.update();

	select_object(selected_object);
}

void level_editor_widget::start_object_editor()
{
	// Set connected signals and slots:
	level_editor.disconnect(&level_editor, 0, this, 0);

	level_editor.connect(&level_editor, SIGNAL(leftMousePressed(int, int)), this, SLOT(on_level_editor_left_mouse_pressed(int, int)));
	level_editor.connect(&level_editor, SIGNAL(leftMouseDragged(int, int)), this, SLOT(on_level_editor_left_mouse_dragged(int, int)));
	level_editor.connect(&level_editor, SIGNAL(leftMouseReleased(int, int)), this, SLOT(on_level_editor_left_mouse_released(int, int)));

	level_editor.connect(&level_editor, SIGNAL(rightMousePressed(int, int)), this, SLOT(on_level_editor_right_mouse_pressed(int, int)));
	level_editor.connect(&level_editor, SIGNAL(rightMouseDragged(int, int)), this, SLOT(on_level_editor_right_mouse_dragged(int, int)));
	level_editor.connect(&level_editor, SIGNAL(rightMouseReleased(int, int)), this, SLOT(on_level_editor_right_mouse_released(int, int)));

	level_editor.connect(&level_editor, SIGNAL(topCursorDragged(int, int)), this, SLOT(on_level_editor_top_cursor_dragged(int, int)));
	level_editor.connect(&level_editor, SIGNAL(bottomCursorDragged(int, int)), this, SLOT(on_level_editor_bottom_cursor_dragged(int, int)));

	level_editor.connect(&level_editor, SIGNAL(bottomCursorReleased(int, int)), this, SLOT(on_level_editor_cursor_released(int, int)));
	level_editor.connect(&level_editor, SIGNAL(topCursorReleased(int, int)), this, SLOT(on_level_editor_cursor_released(int, int)));
}

void level_editor_widget::start_sprite_editor()
{
	// Set connected signals and slots:
	level_editor.disconnect(&level_editor, 0, this, 0);

	level_editor.connect(&level_editor, SIGNAL(leftMousePressed(int, int)), this, SLOT(on_level_editor_left_mouse_pressed_sprites(int, int)));
	level_editor.connect(&level_editor, SIGNAL(leftMouseDragged(int, int)), this, SLOT(on_level_editor_left_mouse_dragged_sprites(int, int)));
	level_editor.connect(&level_editor, SIGNAL(leftMouseReleased(int, int)), this, SLOT(on_level_editor_left_mouse_released_sprites(int, int)));

	level_editor.connect(&level_editor, SIGNAL(rightMousePressed(int, int)), this, SLOT(on_level_editor_right_mouse_pressed_sprites(int, int)));
	level_editor.connect(&level_editor, SIGNAL(rightMouseDragged(int, int)), this, SLOT(on_level_editor_right_mouse_dragged_sprites(int, int)));
	level_editor.connect(&level_editor, SIGNAL(rightMouseReleased(int, int)), this, SLOT(on_level_editor_right_mouse_released_sprites(int, int)));

	level_editor.connect(&level_editor, SIGNAL(topCursorDragged(int, int)), this, SLOT(on_level_editor_left_mouse_dragged_sprites(int, int)));
	level_editor.connect(&level_editor, SIGNAL(bottomCursorDragged(int, int)), this, SLOT(on_level_editor_left_mouse_dragged_sprites(int, int)));

	level_editor.connect(&level_editor, SIGNAL(bottomCursorReleased(int, int)), this, SLOT(on_level_editor_left_mouse_released_sprites(int, int)));
	level_editor.connect(&level_editor, SIGNAL(topCursorReleased(int, int)), this, SLOT(on_level_editor_left_mouse_released_sprites(int, int)));
}

void level_editor_widget::on_action_edit_layer_1_triggered()
{
	level_editor.makeCurrent();

	current_layer = layer_1;
	action_edit_layer_1->setChecked(true);
	action_edit_layer_2->setChecked(false);
	action_edit_sprites->setChecked(false);

	analyze_object_types();

	// Update the entire level:
	display_level_map(opened_map16, opened_level.level_map[LEVEL_LAYER_1], level_editor.get_tilemap_texture(LAYER_1));
	display_level_map(opened_map16, opened_level.level_map[LEVEL_LAYER_2], level_editor.get_tilemap_texture(LAYER_2));

	start_object_editor();
}

void level_editor_widget::on_action_edit_layer_2_triggered()
{
	level_editor.makeCurrent();

	current_layer = layer_2;
	action_edit_layer_1->setChecked(false);
	action_edit_layer_2->setChecked(true);
	action_edit_sprites->setChecked(false);

	analyze_object_types();

	// Update the entire level:
	display_level_map(opened_map16, opened_level.level_map[LEVEL_LAYER_1], level_editor.get_tilemap_texture(LAYER_2));
	display_level_map(opened_map16, opened_level.level_map[LEVEL_LAYER_2], level_editor.get_tilemap_texture(LAYER_1));

	start_object_editor();
}

void level_editor_widget::on_action_edit_sprites_triggered()
{
	level_editor.makeCurrent();

	clear_object_viewer();

	current_layer = sprites;
	action_edit_layer_1->setChecked(false);
	action_edit_layer_2->setChecked(false);
	action_edit_sprites->setChecked(true);

	analyze_object_types();

	// Update the entire level:
	display_level_map(opened_map16, opened_level.level_map[LEVEL_LAYER_1], level_editor.get_tilemap_texture(LAYER_1));
	display_level_map(opened_map16, opened_level.level_map[LEVEL_LAYER_2], level_editor.get_tilemap_texture(LAYER_2));

	start_sprite_editor();
}

void level_editor_widget::display_object_viewer(const QModelIndex &index)
{
	if (opened_rom.is_open() == false)
		return;

	level_editor.makeCurrent();

	object_viewer.sprite_list.clear_data();

	if(current_layer == sprites)
	{
		if(index.isValid() == false)
			return;

		unsigned char sprite_number = (unsigned char)index.row();

		// Create a temporary sprite:
		opengl_sprite temp_sprite;
		temp_sprite.tilemap = opened_level.toolbox_sprites.items[sprite_number].opengl_tilemap;
		temp_sprite.pos_x = 8 * 16 - opened_level.toolbox_sprites.items[sprite_number].size_x * 4;
		temp_sprite.pos_y = 2 * 16 - opened_level.toolbox_sprites.items[sprite_number].size_y * 4;
		temp_sprite.size_x = opened_level.toolbox_sprites.items[sprite_number].size_x;
		temp_sprite.size_y = opened_level.toolbox_sprites.items[sprite_number].size_y;

		object_viewer.sprite_list.add_item(temp_sprite);

		object_viewer.moveScrollX(-object_viewer.get_width() / 2 / 4 + 32);
		object_viewer.update();
	} else {
		// If the selected index is a sub, then this index holds the object subtype and the parent holds the object type:
		bool is_sub = index.parent().isValid();

		// Preview the object in the preview box:

		// Create a temporary object:
		level_object object;
		object.new_screen = 0;
		object.screen_number = 0;
		object.position_X = 6;
		object.position_Y = 0;

		if(is_sub==true)
		{
			object.object_number = index.parent().row();
			object.data_format = opened_level.toolbox.items[object.object_number].format;

			object.set_type(index.row());
		}
		else
		{
			object.object_number = index.row();
			object.data_format = opened_level.toolbox.items[object.object_number].format;
			object.set_type(0);
		}

		object.set_height(3);
		object.set_width(3);

		// Create a temporary empty level for displaying the object in:
		level_data data;
		data.header = opened_level.header;
		data.level_map[0] = level_map_data(32, 16);
		data.level_map[0].clear_data();
		data.object_list[0].clear_data();
		data.object_list[0].add_item(object);
		data.decode_object_data(opened_rom, 0);

		if(data.object_list[0].get_item(0).error != emulation_error::emulator_success)
			emit emit_message(message_type::error, "The selected object type can't be emulated and is likely corrupted.");
		else
			emit emit_message(message_type::notice, "");

		display_level_map(opened_map16, data.level_map[0], object_viewer.get_tilemap_texture(0));

		object_viewer.moveScrollX(-object_viewer.get_width() / 2 / 4 + 32);
		object_viewer.update();

		object_details_label.setText((QString)"Data Format: " + object_data_format_string::string_long[(int)object.data_format]);
	}



}

void level_editor_widget::clear_object_viewer()
{
	level_map_data temp_map = level_map_data(32, 16);
	temp_map.clear_data();

	display_level_map(opened_map16, temp_map, object_viewer.get_tilemap_texture(0));

	object_viewer.moveScrollX(-object_viewer.get_width() / 2 / 4 + 32);
	object_viewer.update();
}

void level_editor_widget::on_insert_object_list_selection_changed(const QModelIndex &index, const QModelIndex &previous)
{


	// Emulate and display the object that is selected in the object viewer:
	display_object_viewer(index);

	// Deselect any object still selected:
	deselect_object();
}

void level_editor_widget::keyPressEvent( QKeyEvent * event )
{
	if(current_layer == sprites)
	{
		qWarning() << "TODO: EDITING SPRITES!";
		return;
	} else
	{

		if( event->key() == Qt::Key_Delete )
		{
			// If the DELETE key was pressed, delete the selected object:
			if(selected_object >= opened_level.object_list[current_layer].get_length())
				return;

			level_editor.makeCurrent();

			opened_level.object_list[current_layer].remove_item(selected_object);

			qWarning() << "Removed Object";

			//ui->object_list->model()->removeRow(selected_object);

			opened_level.update_level(opened_rom, current_layer);

			// Update the entire level:
			if(current_layer == layer_2)
			{
				display_level_map(opened_map16, opened_level.level_map[LEVEL_LAYER_1], level_editor.get_tilemap_texture(LAYER_2));
				display_level_map(opened_map16, opened_level.level_map[LEVEL_LAYER_2], level_editor.get_tilemap_texture(LAYER_1));
			} else
			{
				display_level_map(opened_map16, opened_level.level_map[LEVEL_LAYER_1], level_editor.get_tilemap_texture(LAYER_1));
				display_level_map(opened_map16, opened_level.level_map[LEVEL_LAYER_2], level_editor.get_tilemap_texture(LAYER_2));
			}

			level_editor.update();

			select_object(selected_object);
		} else if(event->key() == Qt::Key_S)
		{
			rom_error error = opened_level.save_level(opened_rom, level_selector->value());

			if(error == rom_error::no_freespace)
			{
				emit emit_message(message_type::error, "Could not save level 0x" + QString("%1").arg(level_selector->value(), 2, 16, QLatin1Char( '0' )).toUpper() + ", as there is no free space! Consider expanding the rom.");
			} else {
				emit emit_message(message_type::notice, "Saved level 0x" + QString("%1").arg(level_selector->value(), 2, 16, QLatin1Char( '0' )).toUpper() + " to address 0x" + QString("%1").arg(opened_level.address_of_level[LEVEL_LAYER_1], 6, 16, QLatin1Char( '0' )).toUpper());
			}




		} else if(event->key() == Qt::Key_R)
		{
			opened_rom.find_rats_freespace(500);

			opened_rom.expand_rom(rom_size::banks_32);
		}

	}

}

//
//
//	SPRITE EDITOR
//
//

void level_editor_widget::on_level_editor_left_mouse_pressed_sprites(int x, int y)
{
	if(current_layer != sprites)
	{
		qWarning() << "TODO: WE'RE NOT EDITING SPRITES!";
		return;
	}

	has_dragged = false;

	if((x < 0) || (x >= opened_level.level_map[LEVEL_LAYER_1].getMapSizeX()) || (y < 0) || (y >= opened_level.level_map[LEVEL_LAYER_1].getMapSizeY()))
		return;

	for(unsigned int a=0; a<opened_level.sprite_list.get_length(); a++)
	{
		if (opened_level.sprite_list.get_item(a).position_x == x &&
			opened_level.sprite_list.get_item(a).position_y == y)
		{
			selected_object = a;

			relative_selection_x = x - opened_level.sprite_list.get_item(a).position_x;
			relative_selection_y = y - opened_level.sprite_list.get_item(a).position_y;

			unsigned char sprite_number = opened_level.sprite_list.get_item(a).sprite_number;

			qWarning() << "Selected sprite" << a << "of type" << sprite_number;

			level_editor.set_selection_cursor_position(opened_level.sprite_list.get_item(selected_object).position_x * 2 + opened_level.toolbox_sprites.items[sprite_number].displacement_x / 8,
													   opened_level.sprite_list.get_item(selected_object).position_y * 2 + opened_level.toolbox_sprites.items[sprite_number].displacement_y / 8);
			level_editor.set_resize_cursor_position((opened_level.sprite_list.get_item(selected_object).position_x * 2 + opened_level.toolbox_sprites.items[sprite_number].size_x  + opened_level.toolbox_sprites.items[sprite_number].displacement_x / 8) - 1,
													(opened_level.sprite_list.get_item(selected_object).position_y * 2 + opened_level.toolbox_sprites.items[sprite_number].size_y  + opened_level.toolbox_sprites.items[sprite_number].displacement_y / 8) - 1);

			if(sprite_number < 0x100)
			{
				QModelIndex selected_node;

				selected_node = insert_object_list.model()->index(sprite_number, 0, QModelIndex());

				insert_object_list.selectionModel()->select(selected_node, QItemSelectionModel::SelectCurrent);
				insert_object_list.scrollTo(selected_node);
				display_object_viewer(selected_node);
			}

			return;
		}
	}

	level_editor.set_selection_cursor_position(-1, -1);
	level_editor.set_resize_cursor_position(-1, -1);

	selected_object = -1;

	qWarning() << "Could not find a sprite" << opened_level.sprite_list.get_length();
}

void level_editor_widget::on_level_editor_left_mouse_dragged_sprites(int x, int y)
{
	if(current_layer != sprites)
	{
		qWarning() << "TODO: WE'RE NOT EDITING SPRITES!";
		return;
	}

	has_dragged = true;

	if(selected_object >= opened_level.sprite_list.get_length())
		return;

	// Make sure that the object does not get dragged beyond the bounds:
	if(x - relative_selection_x < 0)
		x = relative_selection_x;
	else if(x - relative_selection_x >= opened_level.level_map[LEVEL_LAYER_1].getMapSizeX())
		x = opened_level.level_map[LEVEL_LAYER_1].getMapSizeX() - 1 + relative_selection_x;

	if(y - relative_selection_y < 0)
		y = relative_selection_y;
	else if(y - relative_selection_y >= opened_level.level_map[LEVEL_LAYER_1].getMapSizeY())
		y = opened_level.level_map[LEVEL_LAYER_1].getMapSizeY() - 1 + relative_selection_y;

	level_editor.makeCurrent();

	// TODO: Set the screen number as well.
	level_sprite temp_sprite = opened_level.sprite_list.get_item(selected_object);
	temp_sprite.position_x = x - relative_selection_x;
	temp_sprite.position_y = y - relative_selection_y;
	opened_level.sprite_list.set_item(selected_object, temp_sprite);

	// Show the selection and resize cursors move in the editor widget:
	unsigned char sprite_number = opened_level.sprite_list.get_item(selected_object).sprite_number;
	level_editor.set_selection_cursor_position(opened_level.sprite_list.get_item(selected_object).position_x * 2 + opened_level.toolbox_sprites.items[sprite_number].displacement_x / 8,
											   opened_level.sprite_list.get_item(selected_object).position_y * 2 + opened_level.toolbox_sprites.items[sprite_number].displacement_y / 8);
	level_editor.set_resize_cursor_position((opened_level.sprite_list.get_item(selected_object).position_x * 2 + opened_level.toolbox_sprites.items[sprite_number].size_x  + opened_level.toolbox_sprites.items[sprite_number].displacement_x / 8) - 1,
											(opened_level.sprite_list.get_item(selected_object).position_y * 2 + opened_level.toolbox_sprites.items[sprite_number].size_y  + opened_level.toolbox_sprites.items[sprite_number].displacement_y / 8) - 1);

	display_level_sprites(opened_level.sprite_list, level_editor.sprite_list, opened_level.toolbox_sprites);

}

void level_editor_widget::on_level_editor_left_mouse_released_sprites(int x, int y)
{
	// Set focus to "this", so that it can receive keyboard events:
	setFocus();

	if(current_layer != sprites)
	{
		qWarning() << "TODO: WE'RE NOT EDITING SPRITES!";
		return;
	}
}

void level_editor_widget::on_level_editor_right_mouse_pressed_sprites(int x, int y)
{
	if(current_layer == sprites)
	{
		qWarning() << "TODO: WE'RE EDITING SPRITES!";
		return;
	}

	if((x < 0) || (x >= opened_level.level_map[current_layer].getMapSizeX()) || (y < 0) || (y >= opened_level.level_map[current_layer].getMapSizeY()))
		return;

	level_editor.makeCurrent();

	level_object temp_object;

	if(selected_object >= opened_level.object_list[current_layer].get_length())
	{
		if(insert_object_list.currentIndex().row() < 0)
			return;

		// No object selected, so insert one from the "insert" window:

		// If the selected index is a sub, then this index holds the object subtype and the parent holds the object type:
		bool is_sub = insert_object_list.currentIndex().parent().isValid();

		if(is_sub==true)
		{
			temp_object.object_number = insert_object_list.currentIndex().parent().row();
			temp_object.data_format = opened_level.toolbox.items[temp_object.object_number].format;

			temp_object.set_type(insert_object_list.currentIndex().row());
		}
		else
		{
			temp_object.object_number = insert_object_list.currentIndex().row();
			temp_object.data_format = opened_level.toolbox.items[temp_object.object_number].format;

			temp_object.set_type(0);
		}

		temp_object.set_height(2);
		temp_object.set_width(2);


	} else {
		// Object selected, so copy it:
		temp_object = opened_level.object_list[current_layer].get_item(selected_object);
	}

	// TODO: remove the next hard-coded disability:

	// If the object is a Screen Exit or a Screen Move object, do not add it:
	if((temp_object.object_number == 0x00) && (temp_object.settings_byte_1 == 0x00 || temp_object.settings_byte_1 == 0x01 ))
	{
		emit emit_message(message_type::error, "Cannot manually add a \"Screen Exit\" or a \"Jump To Screen\" object.");
		return;
	}



	opened_level.object_list[current_layer].add_item(temp_object);

	// Calculate the index for the list (this one is purposefully different from the previous time the index was calculated, two lines above):
	unsigned int object_index = opened_level.object_list[current_layer].get_length() - 1;

	// Move the object to the appropriate position:
	opened_level.move_object_x(current_layer, object_index, x);
	opened_level.move_object_y(current_layer, object_index, y);

	// Add the new object to the list in the UI:
	//object_list_items.appendRow(new QStandardItem(QString("%1").arg(object_index, 2, 16, QLatin1Char( '0' )).toUpper() + ": " + QString("%1").arg(opened_level.object_list[LEVEL_LAYER_1].get_item(object_index).object_number, 2, 16, QLatin1Char( '0' )).toUpper()));

	select_object(object_index);

	// Emulate and update the changes:
	opened_level.update_level(opened_rom, current_layer);
	display_level_map(opened_map16, opened_level.level_map[current_layer], level_editor.get_tilemap_texture(LAYER_1));
	level_editor.update();
}

void level_editor_widget::on_level_editor_right_mouse_dragged_sprites(int x, int y)
{

}

void level_editor_widget::on_level_editor_right_mouse_released_sprites(int x, int y)
{
	setFocus();
}
