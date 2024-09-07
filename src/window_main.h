#ifndef WINDOW_MAIN_H
#define WINDOW_MAIN_H

#include "main.h"
#include "editor_ui/level_editor_widget.h"
#include "editor_ui/gfx_editor_widget.h"
#include "editor_ui/rats_viewer_widget.h"
#include "editor_ui/level_list_viewer_widget.h"
#include <QMainWindow>
#include "ui_rendering/utilities/fade_label.h"
#include "editor_ui/ui_enums.h"

#warning "We included file window_main.h"

namespace Ui {
class window_main;
}

class window_main : public QMainWindow
{
    Q_OBJECT

public:
    explicit window_main(QWidget *parent = 0);
    ~window_main();



private:
    Ui::window_main *ui;

	FadeLabel statusbar_label;

	level_editor_widget level_editor_frame;
	gfx_editor_widget gfx_editor_frame;
	rats_viewer_widget rats_viewer_frame;
	level_list_viewer_widget level_list_viewer_frame;

	window_gfx_editor w_gfx_editor;
	window_help w_help;

	// Internal stuff:
	SMW_ROM opened_rom;

	//level_data opened_level;
	//map16_map opened_map16;

	int selected_object;
	int relative_selection_x;
	int relative_selection_y;
	bool has_dragged;						// These variables help make the editing process smoother.

	bool emulate_lock = false;				// When true, this will stop any emulation. This is used to make the editing process smoother when editing the values of GUI items.

	QStandardItemModel object_list_items;
	QStandardItemModel object_types_items;

protected:
	void keyPressEvent( QKeyEvent * event );

private slots:
	void on_action_menu_open_rom_triggered();

	void on_actionGFX_editor_triggered();

	void on_actionAbout_triggered();

	void show_message(message_type type, QString message);
	void on_action_menu_save_rom_triggered();
	void on_tabWidget_currentChanged(int index);
	void on_actionExpand_to_1MB_triggered();
	void on_actionExpand_to_2MB_triggered();
	void on_actionExpand_to_4MB_triggered();
};

#endif // WINDOW_MAIN_H
