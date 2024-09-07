#ifndef WINDOW_HELP_H
#define WINDOW_HELP_H

#include <QDialog>

namespace Ui {
class window_help;
}

class window_help : public QDialog
{
	Q_OBJECT

public:
	explicit window_help(QWidget *parent = 0);
	~window_help();

private:
	Ui::window_help *ui;
};

#endif // WINDOW_HELP_H
