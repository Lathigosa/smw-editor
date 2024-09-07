#ifndef TEXTURES_H
#define TEXTURES_H

// This function returns the openGL index of an empty texture.
// The interpolation quality is set to 'nearest neighbour', which
//   means that the pixels won't be smoothed down and will remain
//   blocky.
// size_x: The width of the texture in pixels.
// size_y: The height of the texture in pixels.
GLuint generate_empty_texture(int size_x, int size_y);

// This function gives the openGL index of a new, empty texture
//   and a new framebuffer set to write to the texture.
// Useful for when you need a texture that can be written to,
//   such as a texture containing the map16 data.
// The interpolation quality is set to 'nearest neighbour', which
//   means that the pixels won't be smoothed down and will remain
//   blocky.
// size_x: The width of the texture in pixels.
// size_y: The height of the texture in pixels.
// &texture: Returns the index of the new texture.
// &framebuffer: Returns the index of the new framebuffer.
// Returns false if failed, true if succeeded.
bool generate_canvas_framebuffer(GLuint size_x, GLuint size_y, GLuint &texture, GLuint &framebuffer);

#endif // TEXTURES_H
