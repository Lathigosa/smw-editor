#include "main.h"
#include "window_main.h"
#include "editor_ui/level_editor_widget.h"
#include "ui_window_main.h"

window_main::window_main(QWidget *parent) :
	QMainWindow(parent),
	ui(new Ui::window_main),
	statusbar_label(this),
	level_editor_frame(opened_rom, this),
	gfx_editor_frame(opened_rom, this),
	rats_viewer_frame(opened_rom, this),
	level_list_viewer_frame(opened_rom, this)
{
	ui->setupUi(this);

	QFont font;
	font.setFamily(QString::fromUtf8("Noto Sans"));
	statusbar_label.setFont(font);
	statusbar_label.setAlignment(Qt::AlignHCenter);

	ui->statusBar->addWidget(&statusbar_label, 1);

	// Connect signals:
	level_editor_frame.connect(&level_editor_frame, SIGNAL(emit_message(message_type, QString)), this, SLOT(show_message(message_type, QString)));
	gfx_editor_frame.connect(&gfx_editor_frame, SIGNAL(emit_message(message_type, QString)), this, SLOT(show_message(message_type, QString)));
	rats_viewer_frame.connect(&rats_viewer_frame, SIGNAL(emit_message(message_type, QString)), this, SLOT(show_message(message_type, QString)));
	level_list_viewer_frame.connect(&level_list_viewer_frame, SIGNAL(emit_message(message_type, QString)), this, SLOT(show_message(message_type, QString)));

	// Add editor frame widgets to the tabpages:
	ui->tab_level_editor->layout()->addWidget(&level_editor_frame);
	ui->tab_gfx_editor->layout()->addWidget(&gfx_editor_frame);
	ui->tab_rats_viewer->layout()->addWidget(&rats_viewer_frame);
	ui->tab_level_list_viewer->layout()->addWidget(&level_list_viewer_frame);

	// Show the toolbar of the selected tab:
	on_tabWidget_currentChanged(ui->tabWidget->currentIndex());

	// Fill the object types list (in the 'Insert' menu and 'Object Settings' setting).
	int a;
	for (a=0; a<0x40; a++) {
		// This can be 'new', since 'delete' will be called on removeRow anyway. This is not a memory leak.
		object_types_items.appendRow(new QStandardItem(QString("%1").arg(a, 2, 16, QLatin1Char( '0' )).toUpper() + ": Object"));
	}
}

window_main::~window_main()
{
	delete ui;
}

void window_main::keyPressEvent( QKeyEvent * event )
{

}

void window_main::on_action_menu_open_rom_triggered()
{
	// Load a ROM file:
	QString filename_string = QFileDialog::getOpenFileName(this, tr("Open File"), "", tr("SNES ROM Files (*.smc *.sfc)"));

	if(filename_string.isEmpty() == true)
		return;

	const char * filename = filename_string.toLatin1().data();

	opened_rom.set_filename(filename);

	file_error get_error = opened_rom.load_rom();
	if (get_error != file_error::file_ok)
	{
		show_message(message_type::error, "Could not load file " + filename_string.split("/").last() + ": " + file_error_string::string_long[(int)get_error]);
		return;
	}

	// Update all editor frames:
	level_editor_frame.on_rom_load();
	gfx_editor_frame.on_rom_load();
	rats_viewer_frame.on_rom_load();
	level_list_viewer_frame.on_rom_load();

	show_message(message_type::notice, "Loaded the ROM file " + filename_string.split("/").last());
}

void window_main::on_action_menu_save_rom_triggered()
{
	// Save the ROM file:
	file_error error = opened_rom.save_rom();

	if(error != file_error::file_ok)
	{
		show_message(message_type::error, "Could not save file: " + QString(file_error_string::string_long[(int)error]));
	} else {
		show_message(message_type::notice, "Saved the opened ROM file.");
	}
}

void window_main::on_actionGFX_editor_triggered()
{
	w_gfx_editor.show();
}

void window_main::on_actionAbout_triggered()
{
	w_help.show();
}

void window_main::show_message(message_type type, QString message)
{
	statusbar_label.set_color(QColor(0, 0, 0, 0));
	QString output_string;

	if(type == message_type::notice) {
		output_string = message;
		statusbar_label.fade_arc_and_hide(QColor(0, 0, 0, 255), QColor(0, 0, 0, 255), QColor(0, 0, 0, 0), 2000);
	} else if(type == message_type::warning) {
		output_string = "WARNING: " + message;
		statusbar_label.fade_arc_and_hide(QColor(192, 128, 32, 255), QColor(0, 0, 0, 255), QColor(0, 0, 0, 0), 5000);
	} else if(type == message_type::error) {
		output_string = "ERROR: " + message;
		statusbar_label.fade_arc_and_hide(QColor(255, 0, 0, 255), QColor(0, 0, 0, 255), QColor(0, 0, 0, 0), 5000);
	}

	statusbar_label.setText(output_string);
}

// This function changes the visible tab widgets.
void window_main::on_tabWidget_currentChanged(int index)
{
	level_editor_frame.stop_animation();
	gfx_editor_frame.stop_animation();
	rats_viewer_frame.stop_animation();
	level_list_viewer_frame.stop_animation();

	if(index == 0)
	{
		level_editor_frame.show_toolbar_items(*ui->mainToolBar);
		level_editor_frame.start_animation();
	} else if(index == 1)
	{
		gfx_editor_frame.show_toolbar_items(*ui->mainToolBar);
		gfx_editor_frame.start_animation();
	} else if(index == 2)
	{
		rats_viewer_frame.show_toolbar_items(*ui->mainToolBar);
		rats_viewer_frame.start_animation();
	} else if(index == 3)
	{
		level_list_viewer_frame.show_toolbar_items(*ui->mainToolBar);
		level_list_viewer_frame.start_animation();
	}
	else
	{
		ui->mainToolBar->clear();
	}
}




void window_main::on_actionExpand_to_1MB_triggered()
{
	opened_rom.expand_rom(rom_size::banks_32);
	show_message(message_type::notice, "Expanded the opened rom to 1MB.");
}

void window_main::on_actionExpand_to_2MB_triggered()
{
	opened_rom.expand_rom(rom_size::banks_64);
	show_message(message_type::notice, "Expanded the opened rom to 2MB.");
}

void window_main::on_actionExpand_to_4MB_triggered()
{
	opened_rom.expand_rom(rom_size::banks_128);
	show_message(message_type::notice, "Expanded the opened rom to 4MB.");
}
