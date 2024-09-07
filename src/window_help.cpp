#include "window_help.h"
#include "ui_window_help.h"

window_help::window_help(QWidget *parent) :
	QDialog(parent),
	ui(new Ui::window_help)
{
	ui->setupUi(this);
}

window_help::~window_help()
{
	delete ui;
}
