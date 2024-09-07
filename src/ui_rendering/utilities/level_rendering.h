#ifndef LEVEL_RENDERING_H
#define LEVEL_RENDERING_H

#include <QtOpenGL>

struct opengl_layer
{
	GLuint tilemap_texture;
	float opacity;
};

struct opengl_sprite
{
	GLuint tilemap;
	int pos_x;
	int pos_y;
	int size_x;
	int size_y;
};

#endif // LEVEL_RENDERING_H
