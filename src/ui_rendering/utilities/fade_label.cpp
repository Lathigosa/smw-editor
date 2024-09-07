#include "ui_rendering/utilities/fade_label.h"

FadeLabel::FadeLabel(QWidget *parent) :
	QLabel(parent)
{
}

void FadeLabel::fade_arc_and_hide(QColor color_highlight, QColor color_neutral, QColor color_hidden, int display_time)
{
	previous_color = palette().color(foregroundRole());
	next_color = color_highlight;

	normal_color = color_neutral;
	hidden_color = color_hidden;

	fade_stage = 0;
	time_to_display = display_time;

	frame_counter = 40;
	total_time = 40;
	timer_id = startTimer(16);
}

void FadeLabel::timerEvent(QTimerEvent *event)
{
	QPalette new_palette;
	new_palette.setColor(foregroundRole(), QColor(previous_color.red() * ((float)frame_counter / (float)total_time) + next_color.red() * (1.0 - ((float)frame_counter / (float)total_time)),
												previous_color.green() * ((float)frame_counter / (float)total_time) + next_color.green() * (1.0 - ((float)frame_counter / (float)total_time)),
												previous_color.blue() * ((float)frame_counter / (float)total_time) + next_color.blue() * (1.0 - ((float)frame_counter / (float)total_time)),
												previous_color.alpha() * ((float)frame_counter / (float)total_time) + next_color.alpha() * (1.0 - ((float)frame_counter / (float)total_time))));

	setPalette(new_palette);

	if(frame_counter <= 0)
	{
		if(fade_stage == 0)				// Hold the highlight color for a while.
		{
			frame_counter = 1;
			total_time = 1;

			killTimer(timer_id);
			timer_id = startTimer(75);
			fade_stage = 1;
		} else if(fade_stage == 1)		// Fade to the normal color.
		{
			frame_counter = 70;
			total_time = 70;

			fade_stage = 2;

			previous_color = next_color;
			next_color = normal_color;
		} else if(fade_stage == 2)		// Hold the normal color for a while.
		{
			frame_counter = 1;
			total_time = 1;

			killTimer(timer_id);
			timer_id = startTimer(time_to_display);
			fade_stage = 3;
		} else if(fade_stage == 3)		// Fade to the hidden color.
		{
			frame_counter = 100;
			total_time = 100;

			killTimer(timer_id);
			timer_id = startTimer(35);
			fade_stage = 4;

			previous_color = normal_color;
			next_color = hidden_color;
		} else							// Animation finished, kill the timer.
		{
			killTimer(timer_id);
		}
	}

	frame_counter--;
}

void FadeLabel::set_color(QColor color)
{
	killTimer(timer_id);

	QPalette new_palette;
	new_palette.setColor(foregroundRole(), color);

	setPalette(new_palette);
}
