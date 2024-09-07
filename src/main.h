#ifndef MAIN_H
#define MAIN_H

#define GL_GLEXT_PROTOTYPES

#include <iostream>
#include <fstream>
#include <cstdio>
#include <cmath>

#include <QtOpenGL>

#include <QWidget>
#include <QGLWidget>
#include <QGLShaderProgram>
#include <QGLBuffer>

#define NUMBER_OF_GFX_PAGES_IN_MEMORY 8

#include "utilities/address.h"
#include "utilities/buffer.h"

#include "error_rom/error_includes.h"

#include "load_rom/load_rom.h"

#include "format_rom/format_includes.h"
#include "display_rom/display_includes.h"

#include "load_rom/definitions_rom.h"



#include "emulate_rom/emulator.h"
#include "load_rom/utilities/emulate_object.h"
#include "load_rom/utilities/compression.h"
#include "load_rom/utilities/header_test.h"
#include "load_rom/utilities/load_file.h"

#include "load_rom/load_GFX.h"
#include "load_rom/load_level.h"
#include "load_rom/load_map16.h"
#include "load_rom/load_palette.h"

#include "animate_rom/animate_includes.h"


#include "widget_gl_level_editor.h"
#include "widget_gl_gfx_editor.h"
#include "window_gfx_editor.h"
#include "window_help.h"

#include "ui_rendering/utilities/shaders.h"
#include "ui_rendering/utilities/textures.h"
#include "ui_rendering/utilities/hex_spinbox.h"

//#include "editor_ui/level_editor_widget.h"

//#include "window_main.h"

#endif // MAIN_H
