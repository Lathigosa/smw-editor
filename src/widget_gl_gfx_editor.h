#ifndef WIDGET_GL_GFX_EDITOR_H
#define WIDGET_GL_GFX_EDITOR_H

#include "main.h"

#include "ui_rendering/utilities/level_rendering.h"

namespace Ui {
class widget_gl_gfx_editor;
}

class widget_gl_gfx_editor : public QGLWidget
{
	Q_OBJECT
public:
	widget_gl_gfx_editor(QWidget* parent = 0 , const widget_gl_gfx_editor *shareWidget = NULL);
	~widget_gl_gfx_editor();

	GLuint m_selection_gfx_texture = 0;
	GLuint m_terrain_gfx_texture = 0;
	GLuint m_sprite_gfx_texture = 0;
	GLuint m_palette_texture = 0;

	GLuint get_tilemap_texture(unsigned int index);
	void set_tilemap_opacity(unsigned int index, float opacity);

	//queue<opengl_layer> layer_list;

	queue<opengl_sprite> sprite_list;

	void set_frame_size(int x, int y, unsigned int amount_of_layers);

	void moveScrollX(int X);
	void moveScrollY(int Y);
	void setZoom(float zoom_level);

	void start_animation(const SMW_ROM &rom, unsigned char tileset);
	void stop_animation();

	void set_selection_cursor_position(int x, int y);
	void set_resize_cursor_position(int x, int y);

	int get_width() const
	{
		return width;
	}

	int get_height() const
	{
		return height;
	}

	int get_visible_tiles_width() const;
	int get_visible_tiles_height() const;
	int get_visible_tiles_x() const;
	int get_visible_tiles_y() const;

	// Override:
	void makeCurrent();

private slots:
	void initialize_non_host(const widget_gl_gfx_editor *shareWidget);

	void update_matrices();

protected:
	virtual void initializeGL();
	virtual void resizeGL( int w, int h );
	virtual void paintGL();
	void mousePressEvent(QMouseEvent * event);
	void mouseMoveEvent(QMouseEvent * event);
	void mouseReleaseEvent(QMouseEvent * event);

	//virtual void keyPressEvent( QKeyEvent* e );
signals:
	void leftMousePressed(int x, int y);
	void leftMouseDragged(int x, int y);
	void leftMouseReleased(int x, int y);
	void rightMousePressed(int x, int y);
	void rightMouseDragged(int x, int y);
	void rightMouseReleased(int x, int y);

	void topCursorPressed(int x, int y);				// The signal when the selection cursor triangle is pressed.
	void topCursorDragged(int x, int y);				// The signal when the selection cursor triangle is dragged.
	void topCursorReleased(int x, int y);				// The signal when the selection cursor triangle is released.
	void bottomCursorPressed(int x, int y);				// The signal when the resize cursor triangle is pressed.
	void bottomCursorDragged(int x, int y);				// The signal when the resize cursor triangle is dragged.
	void bottomCursorReleased(int x, int y);			// The signal when the resize cursor triangle is released.
	void finishedInitialization(const widget_gl_gfx_editor *shareWidget);
	void finishedEmulationFrame();

private:
	int previous_mouse_x;		// Used when dragging to calculate the delta values.
	int previous_mouse_y;		// Used when dragging to calculate the delta values.

	//bool prepareShaderProgram( const QString& vertexShaderPath,
	//    const QString& fragmentShaderPath );

	void set_number_of_layers(unsigned int amount);

	unsigned int number_of_layers = 0;
	GLuint * tilemap_textures = NULL;
	float * layer_transparency = NULL;

	QGLShaderProgram * m_shader;
	QGLBuffer m_vertexBuffer;

	QMatrix4x4 matrix;
	QMatrix4x4 matrix_selection;	// The matrix containing the position of the selection indicator.
	QMatrix4x4 matrix_resize;		// The matrix containing the position of the resize cursor indicator.

	//opengl_sprite * sprites;

	int map_tile_size_x;		// The size of the shown map in pixels.
	int map_tile_size_y;		// The size of the shown map in pixels.

	int scroll_x;				// The amount of tiles scrolled left or right.
	int scroll_y;				// The amount of tiles scrolled up or down.

	float scale;				// The zoom factor.

	int width;
	int height;

	// Cursors:
	int selection_cursor_x;
	int selection_cursor_y;
	int resize_cursor_x;
	int resize_cursor_y;

	bool top_cursor_selected = false;
	bool bottom_cursor_selected = false;

	// For animation purposes:
protected:
	void timerEvent(QTimerEvent * event);
	//void paintEvent(QPaintEvent * event);
private:

	int timerId;
	object_emulator * animation_emulator;
	unsigned char frame_counter;

	bool host_animation;		// If true, this widget will host the emulation of the animation and will control all the textures.
};



#endif // WIDGET_GL_GFX_EDITOR_H
