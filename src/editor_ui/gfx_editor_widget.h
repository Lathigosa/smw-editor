#ifndef GFX_EDITOR_WIDGET_H
#define GFX_EDITOR_WIDGET_H

#include "save_rom/encode_level.h"
#include <QWidget>
#include "editor_ui/ui_enums.h"
#include "editor_ui/level_editor_widget.h"	//TODO: remove!!!

#warning "We included file gfx_editor_widget.h"

namespace ui {
class gfx_editor_widget;
}

class gfx_editor_widget : public QWidget
{
	Q_OBJECT
public:
	explicit gfx_editor_widget(SMW_ROM & rom, QWidget *parent = 0);
	void show_toolbar_items(QToolBar & toolbar, QWidget *parent = 0);
	void stop_animation();
	void start_animation();
	void on_rom_load();

private:
	bool is_active = false;

	// Actions in Toolbar:

	// Widgets:
	QGridLayout central_layout;
	QSplitter main_splitter;
	QWidget insert_panel;
	QVBoxLayout insert_panel_layout;
	QLabel insert_panel_title;
	QTreeView insert_object_list;

	QWidget editor_panel;
	QGridLayout editor_panel_layout;
	QScrollBar editor_panel_v_scrollbar;
	QScrollBar editor_panel_h_scrollbar;

	widget_gl_gfx_editor level_editor;
	widget_gl_gfx_editor object_viewer;

	// Toolbar stuff:
	QAction * action_return = NULL;

	QWidget * spacer_widget = NULL;		// The spacer for the toolbar, to keep things before the spacer on the left and things after the spacer on the right.

	HexSpinBox * level_selector;			// The spinbox showing and controlling the currently selected level.
	QSlider * zoom_slider;				// The slider showing and controlling the current zoom level.
	QAction * action_move_up = NULL;
	QAction * action_move_down = NULL;
	QAction * action_move_to_top = NULL;
	QAction * action_move_to_bottom = NULL;

	QAction * action_edit_layer_1 = NULL;
	QAction * action_edit_layer_2 = NULL;
	QAction * action_edit_sprites = NULL;

	editing_layer current_layer = layer_1;

	void setup_ui();

	// Internal Data:
	SMW_ROM &opened_rom;

	level_data opened_level_old[2];
	level_data opened_level;
	map16_map opened_map16;

	void calculate_scrollbar_size();

	bool select_object(unsigned int object_index);
	bool deselect_object();

	bool analyze_object_types();

	void start_object_editor();
	void start_sprite_editor();

	int selected_object;
	int relative_selection_x;
	int relative_selection_y;
	bool has_dragged;						// These variables help make the editing process smoother.

	bool emulate_lock = false;				// When true, this will stop any emulation. This is used to make the editing process smoother when editing the values of GUI items.

	QStandardItemModel object_list_items;
	QStandardItemModel object_types_items;

	void display_object_viewer(const QModelIndex &index);
	void clear_object_viewer();

protected:
	void keyPressEvent( QKeyEvent * event );
	void resizeEvent(QResizeEvent * event);

public slots:
	void select_gfx_file(int index);

	void on_v_scrollbar_slider_moved(int position);
	void on_h_scrollbar_slider_moved(int position);
	void on_zoom_slider_slider_moved(int position);

	// Objects editor:
	void on_level_editor_left_mouse_pressed(int x, int y);
	void on_level_editor_left_mouse_dragged(int x, int y);
	void on_level_editor_left_mouse_released(int x, int y);
	void on_level_editor_right_mouse_pressed(int x, int y);
	void on_level_editor_right_mouse_dragged(int x, int y);
	void on_level_editor_right_mouse_released(int x, int y);

	void on_level_editor_top_cursor_dragged(int x, int y);
	void on_level_editor_bottom_cursor_dragged(int x, int y);
	void on_level_editor_cursor_released(int x, int y);

	// Sprites editor:
	void on_level_editor_left_mouse_pressed_sprites(int x, int y);
	void on_level_editor_left_mouse_dragged_sprites(int x, int y);
	void on_level_editor_left_mouse_released_sprites(int x, int y);
	void on_level_editor_right_mouse_pressed_sprites(int x, int y);
	void on_level_editor_right_mouse_dragged_sprites(int x, int y);
	void on_level_editor_right_mouse_released_sprites(int x, int y);

	void on_main_splitter_moved(int pos, int index);

	void on_insert_object_list_selection_changed(const QModelIndex &index, const QModelIndex &previous);

	void on_action_move_up_triggered();
	void on_action_move_down_triggered();
	void on_action_move_to_top_triggered();
	void on_action_move_to_bottom_triggered();

	void on_action_edit_layer_1_triggered();
	void on_action_edit_layer_2_triggered();
	void on_action_edit_sprites_triggered();

signals:
	void emit_message(message_type type, QString message);
};

#endif // GFX_EDITOR_WIDGET_H

