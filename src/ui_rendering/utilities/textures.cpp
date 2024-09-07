#include "main.h"

// This function returns the openGL index of an empty texture.
// The interpolation quality is set to 'nearest neighbour', which
//   means that the pixels won't be smoothed down and will remain
//   blocky.
// size_x: The width of the texture in pixels.
// size_y: The height of the texture in pixels.
GLuint generate_empty_texture(int size_x, int size_y) {
    GLuint generatedTexture;
    glGenTextures(1, &generatedTexture);

    glBindTexture(GL_TEXTURE_2D, generatedTexture);

    glTexImage2D(GL_TEXTURE_2D, 0,GL_RGBA, size_x, size_y, 0,GL_RGBA, GL_UNSIGNED_BYTE, 0);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

    return generatedTexture;
}

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
bool generate_canvas_framebuffer(GLuint size_x, GLuint size_y, GLuint &texture, GLuint &framebuffer)
{
    // The framebuffer, which regroups 0, 1, or more textures, and 0 or 1 depth buffer.
    GLuint FramebufferName = 0;
    glGenFramebuffers(1, &FramebufferName);
    glBindFramebuffer(GL_FRAMEBUFFER, FramebufferName);

    // The texture we're going to render to
    GLuint renderedTexture;
    glGenTextures(1, &renderedTexture);

    // "Bind" the newly created texture : all future texture functions will modify this texture
    glBindTexture(GL_TEXTURE_2D, renderedTexture);

    // Give an empty image to OpenGL ( the last "0" )
    glTexImage2D(GL_TEXTURE_2D, 0,GL_RGBA, size_x, size_y, 0,GL_RGBA, GL_UNSIGNED_BYTE, 0);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

    // The depth buffer
    GLuint depthrenderbuffer;
    glGenRenderbuffers(1, &depthrenderbuffer);
    glBindRenderbuffer(GL_RENDERBUFFER, depthrenderbuffer);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, size_x, size_y);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthrenderbuffer);

    // Set "renderedTexture" as our colour attachement #0
    glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, renderedTexture, 0);

    // Set the list of draw buffers.
    GLenum DrawBuffers[1] = {GL_COLOR_ATTACHMENT0};
    glDrawBuffers(1, DrawBuffers); // "1" is the size of DrawBuffers

    // Always check that our framebuffer is ok
    if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        //std::cout << "Framebuffer Failed!";
        return false;
    }

    framebuffer = FramebufferName;
    texture = renderedTexture;

    //glEnable(GL_BLEND);
    //glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    return true;
}
