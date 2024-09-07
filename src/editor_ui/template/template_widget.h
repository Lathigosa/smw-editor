#ifndef RATS_VIEWER_WIDGET_H
#define RATS_VIEWER_WIDGET_H

class rats_viewer_widget : public QWidget
{
	Q_OBJECT
public:
	explicit rats_viewer_widget(SMW_ROM & rom, QWidget *parent = 0);
	void show_toolbar_items(QToolBar & toolbar, QWidget *parent = 0);
	void stop_animation();
	void start_animation();
	void on_rom_load();

signals:
	void emit_message(message_type type, QString message);
};

#endif // RATS_VIEWER_WIDGET_H
