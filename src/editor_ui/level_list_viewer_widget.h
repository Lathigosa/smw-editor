#ifndef LEVEL_LIST_VIEWER_WIDGET_H
#define LEVEL_LIST_VIEWER_WIDGET_H

#include <QWidget>
#include <QToolBar>
#include <QGridLayout>
#include <QSplitter>
#include <QTreeView>
#include <QStandardItemModel>

#include "load_rom/load_rom.h"
#include "ui_enums.h"

class level_list_viewer_widget : public QWidget
{
	Q_OBJECT
public:
	explicit level_list_viewer_widget(SMW_ROM & rom, QWidget *parent = 0);
	void show_toolbar_items(QToolBar & toolbar, QWidget *parent = 0);
	void stop_animation();
	void start_animation();
	void on_rom_load();

private:
	bool is_active = false;

	void setup_ui();
	void display_rats_tags();

	// Widgets:
	QGridLayout central_layout;
	QSplitter main_splitter;
	QTreeView view_rats_list;

	QStandardItemModel rats_tag_item_list;

	// Internal Data:
	SMW_ROM &opened_rom;

signals:
	void emit_message(message_type type, QString message);
};

#endif // LEVEL_LIST_VIEWER_WIDGET_H
