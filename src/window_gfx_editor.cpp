#include "window_gfx_editor.h"
#include "ui_window_gfx_editor.h"

window_gfx_editor::window_gfx_editor(QWidget *parent) :
	QMainWindow(parent),
	ui(new Ui::window_gfx_editor)
{
	ui->setupUi(this);
}

window_gfx_editor::~window_gfx_editor()
{
	delete ui;
}
