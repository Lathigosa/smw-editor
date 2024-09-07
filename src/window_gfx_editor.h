#ifndef WINDOW_GFX_EDITOR_H
#define WINDOW_GFX_EDITOR_H

#include <QMainWindow>

namespace Ui {
class window_gfx_editor;
}

class window_gfx_editor : public QMainWindow
{
	Q_OBJECT

public:
	explicit window_gfx_editor(QWidget *parent = 0);
	~window_gfx_editor();

private:
	Ui::window_gfx_editor *ui;
};

#endif // WINDOW_GFX_EDITOR_H
