#ifndef FADE_LABEL_H
#define FADE_LABEL_H

#include <QWidget>
#include <QLabel>
#include <QColor>
#include <QPalette>
#include <QTimerEvent>
#include <QDebug>

class FadeLabel : public QLabel
{
	Q_OBJECT
public:
	explicit FadeLabel(QWidget *parent = 0);
	void fade_arc_and_hide(QColor color_highlight, QColor color_neutral, QColor color_hidden, int display_time);
	void set_color(QColor color);

protected:
	void timerEvent(QTimerEvent *event);

private:
	int frame_counter;
	int total_time;
	int timer_id;

	int fade_stage;

	int time_to_display;

	QColor previous_color;
	QColor next_color;

	QColor normal_color;
	QColor hidden_color;
};

#endif // FADE_LABEL_H
