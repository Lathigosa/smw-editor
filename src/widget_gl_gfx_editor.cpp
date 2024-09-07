#include "main.h"

int contextcounter2 = 0;

void widget_gl_gfx_editor::makeCurrent()
{
	qWarning() << "It made gfx current!" << contextcounter2;
	contextcounter2++;
	if(context() != context()->currentContext())
	{
		qWarning() << "Didn't skip!" << contextcounter2;
		QGLWidget::makeCurrent();
	}
}

GLuint generate_single_texture(QGLShaderProgram &shader, int size_x, int size_y, GLint internalFormat) {
	GLuint texture;

	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);

	glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, size_x, size_y, 0, internalFormat, GL_UNSIGNED_BYTE, 0);

	// Blocky filtering:
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

	return texture;
}

widget_gl_gfx_editor::widget_gl_gfx_editor( QWidget* parent, const widget_gl_gfx_editor *shareWidget )		// It can only have the main window as parent.
	: QGLWidget( QGLFormat(QGL::SampleBuffers), parent, shareWidget ),
	  m_vertexBuffer( QGLBuffer::VertexBuffer )
{
	// ===== INITIAL SETTINGS ==== //
	map_tile_size_x = 0;	// 16 * 2 * screens = 1024, the 8x8 tile width of a horizontal level.
	map_tile_size_y = 0;	// 27 * 2 = 54, the 8x8 tile height of a horizontal level.
	scroll_x = 0;
	scroll_y = 0;
	scale = 1.0;
	selection_cursor_x = -1;
	selection_cursor_y = -1;
	resize_cursor_x = -1;
	resize_cursor_y = -1;

	// ===== ANIMATION STUFF ===== //
	animation_emulator = NULL;			// The emulator can only be initialized when a proper ROM is loaded.

	makeCurrent();
	m_shader = new QGLShaderProgram(parent);
	//glInit();

	if(shareWidget == NULL) {
		host_animation = true;

		// If this widget is the host animator, handle animation by itself:
		//timerId = startTimer(20);

	} else {
		host_animation = false;

		// Once the host widget finishes initializing or rendering, perform the appropriate functions:
		shareWidget->connect(shareWidget, SIGNAL(finishedInitialization(const widget_gl_gfx_editor *)), this, SLOT(initialize_non_host(const widget_gl_gfx_editor *)));
		//shareWidget->connect(shareWidget, SIGNAL(finishedEmulationFrame()), this, SLOT(update()));
		timerId = startTimer(160);
	}
}

void widget_gl_gfx_editor::initialize_non_host(const widget_gl_gfx_editor *shareWidget)
{
	m_selection_gfx_texture = shareWidget->m_selection_gfx_texture;
	//m_terrain_gfx_texture = shareWidget->m_terrain_gfx_texture;
	m_sprite_gfx_texture = shareWidget->m_sprite_gfx_texture;
	//m_tilemap_texture = shareWidget->m_tilemap_texture;
	m_palette_texture = shareWidget->m_palette_texture;
}

void widget_gl_gfx_editor::set_frame_size(int x, int y, unsigned int amount_of_layers)
{
	makeCurrent();

	// Make sure we don't reset the tilemap texture for no reason:
	if(((map_tile_size_x == x) && (map_tile_size_y == y)) || (m_terrain_gfx_texture == 0))
		return;

	map_tile_size_x = x;
	map_tile_size_y = y;

	glDeleteTextures(1, &m_terrain_gfx_texture);
	m_terrain_gfx_texture = generate_single_texture(*m_shader, map_tile_size_x*8, map_tile_size_y*8, GL_RED);

	update_matrices();
}

void widget_gl_gfx_editor::set_number_of_layers(unsigned int amount)
{
	// Set layer tilemap array:
	if(tilemap_textures != NULL)
	{
		glDeleteTextures(number_of_layers, tilemap_textures);
		delete[] tilemap_textures;
	}

	number_of_layers = amount;

	tilemap_textures = new GLuint[amount];

	for(unsigned int a=0; a<amount; a++)
	{
		tilemap_textures[a] = generate_single_texture(*m_shader, map_tile_size_x, map_tile_size_y, GL_RGBA);
	}

	// Set layer transparency array:
	if(layer_transparency != NULL)
		delete[] layer_transparency;

	layer_transparency = new float[amount];

	for(unsigned int a=0; a<amount; a++)
	{
		layer_transparency[a] = 1.0f;
	}
}

GLuint widget_gl_gfx_editor::get_tilemap_texture(unsigned int index)
{
	if(index >= number_of_layers)
		return 0;

	if(tilemap_textures == NULL)
		return 0;

	return tilemap_textures[index];
}

void widget_gl_gfx_editor::set_tilemap_opacity(unsigned int index, float opacity)
{
	if(index >= number_of_layers)
		return;

	if(layer_transparency == NULL)
		return;

	layer_transparency[index] = opacity;
}

void widget_gl_gfx_editor::timerEvent(QTimerEvent *event)
{
	qWarning() << "TimerEvent!";

	makeCurrent();

	if(host_animation == true)
	{
		if(animation_emulator == NULL) {
			qDebug() << "Animation Emulator is not initialized...";
			return;
		}
		update_vanilla_gfx_animation(*animation_emulator, frame_counter);
		//frame_counter++;
		//update_vanilla_gfx_animation(*animation_emulator, frame_counter);
	}

	updateGL();
	frame_counter++;

	// Tell other widgets to update too:
	//emit finishedEmulationFrame();
}

void widget_gl_gfx_editor::start_animation(const SMW_ROM & rom, unsigned char tileset)
{
	/*if(host_animation == true) {
		if (animation_emulator != NULL)
		{
			delete animation_emulator;
		} else {

			// If this widget is the host animator, handle animation by itself:
			timerId = startTimer(20);
		}

		animation_emulator = new object_emulator(rom, m_terrain_gfx_texture, m_palette_texture);
		animation_emulator->upload_to_ram(tileset, 0x7E1931);

		// Upload the GFX pages to emulator RAM:
		upload_vanilla_gfx_animation(*animation_emulator, rom);

		// Emulate the first few frames, so there are no glitches:
		for(int a=0; a<8; a++)
		{
			update_vanilla_gfx_animation(*animation_emulator, a);
		}

		frame_counter = 8;
	}*/
}

void widget_gl_gfx_editor::stop_animation()
{
	qWarning() << "Stopped gfx animation!";

	if (animation_emulator == NULL)
		return;

	killTimer(timerId);

	delete animation_emulator;
	animation_emulator = NULL;
}

widget_gl_gfx_editor::~widget_gl_gfx_editor()
{
	if(host_animation == false)
		killTimer(timerId);

	stop_animation();

	if(tilemap_textures != NULL)
	{
		glDeleteTextures(number_of_layers, tilemap_textures);
		delete[] tilemap_textures;
	}

	if(layer_transparency != NULL)
		delete[] layer_transparency;

	delete m_shader;
}

void widget_gl_gfx_editor::moveScrollX(int X)
{
	scroll_x = X;
	update_matrices();
	update();
}

void widget_gl_gfx_editor::moveScrollY(int Y)
{
	scroll_y = Y;
	update_matrices();
	update();
}

void widget_gl_gfx_editor::setZoom(float zoom_level)
{
	scale = zoom_level;
	update_matrices();
	update();
}

void widget_gl_gfx_editor::initializeGL()
{
	//makeCurrent();

	glEnable (GL_BLEND);
	glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	QGLFormat glFormat = QGLWidget::format();
	if ( !glFormat.sampleBuffers() )
		qWarning() << "Could not enable sample buffers";

	qWarning() << "OpenGL Version" << glGetString(GL_VERSION);

	// Set the clear color to white:
	//glClearColor( 1.0f, 1.0f, 1.0f, 1.0f );
	glClearColor(this->palette().color(this->backgroundRole()).redF(),
				 this->palette().color(this->backgroundRole()).greenF(),
				 this->palette().color(this->backgroundRole()).blueF(),
				 1.0f);

	// Prepare the main shader program:
	if ( !prepareShaderProgram(m_shader, "gfx_editor.vert", "gfx_editor.frag" ) )
		return;

	// Create a quad that will be used to display the tiles:
	float points[] = { -1.0f, -1.0f, 0.0f, 1.0f,
					   1.0f,  1.0f, 0.0f, 1.0f,
					   -1.0f,  1.0f, 0.0f, 1.0f,
					   -1.0f, -1.0f, 0.0f, 1.0f,
					   1.0f, -1.0f, 0.0f, 1.0f,
					   1.0f,  1.0f, 0.0f, 1.0f };
	m_vertexBuffer.create();
	m_vertexBuffer.setUsagePattern( QGLBuffer::StaticDraw );
	if ( !m_vertexBuffer.bind() )
	{
		qWarning() << "Could not bind vertex buffer to the context";
		return;
	}
	m_vertexBuffer.allocate( points, 6 * 4 * sizeof( float ) );

	// Bind the shader program:
	if ( !m_shader->bind() )
	{
		qWarning() << "Could not bind shader program to context";
		return;
	}

	// Enable the "vertex" attribute to bind it to our currently bound
	// vertex buffer.
	m_shader->setAttributeBuffer( "i_vertex", GL_FLOAT, 0, 4 );
	m_shader->enableAttributeArray( "i_vertex" );

	qWarning() << glGetError();

	// Initialize the textures to be used, only if this widget is the animation host:
	if(host_animation == true) {
		m_terrain_gfx_texture = generate_single_texture(*m_shader, 256, 256, GL_RED);		// This texture contains all the loaded fg and bg gfx pages.
		m_sprite_gfx_texture = generate_single_texture(*m_shader, 256, 256, GL_RED);		// This texture contains all the loaded sprite gfx pages.
		m_palette_texture = generate_single_texture(*m_shader, 16, 16, GL_RGB);				// This texture contains the palette.
		m_selection_gfx_texture = generate_single_texture(*m_shader, 1, 1, GL_RGBA);		// This texture is completely blank, and serves as a texture for coloring the parts that are selected.

		glBindTexture(GL_TEXTURE_2D, m_selection_gfx_texture);
		// Load the colors for the selection gfx (TODO: create a separate shader for this!)
		unsigned char data[4] = {0xC2, 0x23, 0x12, 0x23};

		glTexSubImage2D(GL_TEXTURE_2D,
						0,
						0,
						0,
						1,
						1,
						GL_RGBA,
						GL_UNSIGNED_BYTE,
						reinterpret_cast<void*>(data));


	}

	qWarning() << glGetError();

	qWarning() << "m_gfx_texture:" << m_terrain_gfx_texture ;

	emit finishedInitialization(this);
}

void widget_gl_gfx_editor::update_matrices()
{
	// Set the matrix of the layers:
	matrix.setToIdentity();

	matrix.scale(8.0 * scale * (float)map_tile_size_x / (float)width, 8.0 * scale * (float)map_tile_size_y / (float)height, 1.0);
	matrix.translate((8.0 * scale * (float)map_tile_size_x - (float)width) / (8.0 * scale * (float)map_tile_size_x), ((float)height - 8.0 * scale * (float)map_tile_size_y) / (8.0 * scale * (float)map_tile_size_y), 0.0);

	matrix.translate((float)-scroll_x / ((float)map_tile_size_x), (float)scroll_y / ((float)map_tile_size_y), 0.0);

	// Set the matrix of the selection cursor:
	matrix_selection.setToIdentity();

	matrix_selection.scale(8.0 * scale / (float)width, 8.0 * scale / (float)height, 1.0);
	matrix_selection.translate((8.0 * scale - (float)width) / (8.0 * scale), ((float)height - 8.0 * scale) / (8.0 * scale), 0.1);
	matrix_selection.translate(2.0 * selection_cursor_x, -2.0 * selection_cursor_y, 0.0);

	matrix_selection.translate((float)-scroll_x, (float)scroll_y, 0.0);

	// Set the matrix of the selection cursor:
	matrix_resize.setToIdentity();

	matrix_resize.scale(8.0 * scale / (float)width, 8.0 * scale / (float)height, 1.0);
	matrix_resize.translate((8.0 * scale - (float)width) / (8.0 * scale), ((float)height - 8.0 * scale) / (8.0 * scale), 0.1);
	matrix_resize.translate(2.0 * resize_cursor_x, -2.0 * resize_cursor_y, 0.0);

	matrix_resize.translate((float)-scroll_x, (float)scroll_y, 0.0);
}

void widget_gl_gfx_editor::resizeGL( int w, int h )
{
	glViewport(0, 0, w, h);

	width = w;
	height = h;

	update_matrices();
}
void widget_gl_gfx_editor::paintGL()
{
	//glUseProgram(m_shader.programId());

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	m_shader->setUniformValue("gfx_texture", 0);
	m_shader->setUniformValue("map_texture", 1);
	m_shader->setUniformValue("palette_texture", 2);

	m_shader->setUniformValue("transparency", 1.0f);

	// Set up the textures and uniforms:
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, m_terrain_gfx_texture);

	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, m_palette_texture);

	//m_shader->setUniformValue("u_Tile_Count", QVector2D((float)map_tile_size_x, (float)map_tile_size_y));
	//m_shader->setUniformValue("u_Matrix", matrix);

	// Render all layers:
	//if(tilemap_textures != NULL && layer_transparency != NULL)
	//{

			//m_shader->setUniformValue("transparency", layer_transparency[a]);

			// TEMP::
				QMatrix4x4 temp_matrix;
				temp_matrix.setToIdentity();
				temp_matrix.scale(8.0 * scale * (float)32 / (float)width, 8.0 * scale * (float)32 / (float)height, 1.0);
				temp_matrix.translate((8.0 * scale * (float)32 - (float)width) / (8.0 * scale * (float)32), ((float)height - 8.0 * scale * (float)32) / (8.0 * scale * (float)32), 0.0);
				//temp_matrix.translate(4.0 * sprite_list.get_item(a).pos_x / (float)32, -4.0 * sprite_list.get_item(a).pos_y / (float)32, 0.0);
				temp_matrix.translate((float)-scroll_x / ((float)32), (float)scroll_y / ((float)32), 0.0);

				//m_shader->setUniformValue("u_Tile_Count", QVector2D((float)32, (float)32));
				//m_shader->setUniformValue("u_Matrix", temp_matrix);
				m_shader->setUniformValue("u_Tile_Count", QVector2D((float)map_tile_size_x, (float)map_tile_size_y));
				m_shader->setUniformValue("u_Matrix", matrix);

			//glActiveTexture(GL_TEXTURE1);
			//glBindTexture(GL_TEXTURE_2D, test_sprite_texture);

			glDrawArrays(GL_TRIANGLES, 0, 6);
	//}

	//qglColor(Qt::black);
	//renderText(0.0f, 0.0f, 0.0f, "Some short string...", this->font() );

	m_shader->setUniformValue("transparency", 0.8f);

	// Render the selection cursor:
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, m_selection_gfx_texture);

	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, m_selection_gfx_texture);

	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, m_selection_gfx_texture);

	m_shader->setUniformValue("u_Matrix", matrix_selection);

	glDrawArrays(GL_TRIANGLES, 0, 3);

	// Render the resize cursor:
	m_shader->setUniformValue("u_Matrix", matrix_resize);

	glDrawArrays(GL_TRIANGLES, 3, 3);

}

void widget_gl_gfx_editor::mousePressEvent(QMouseEvent * event)
{
	int x_tile_16 = (event->x() + ((scroll_x * 4 * scale))) / (16 * scale);
	int y_tile_16 = (event->y() + ((scroll_y * 4 * scale))) / (16 * scale);

	int x_tile_8 = (event->x() + ((scroll_x * 4 * scale))) / (8 * scale);
	int y_tile_8 = (event->y() + ((scroll_y * 4 * scale))) / (8 * scale);

	// Test to see if any of the selection triangles are selected:
	if((selection_cursor_x) == x_tile_8 && (selection_cursor_y == y_tile_8))
		top_cursor_selected = true;
	else if((resize_cursor_x) == x_tile_8 && (resize_cursor_y == y_tile_8))
		bottom_cursor_selected = true;

	// Emit the proper signal:
	if(event->button() == Qt::LeftButton)
	{
		if(top_cursor_selected == true)
			emit topCursorPressed(x_tile_16, y_tile_16);
		else if(bottom_cursor_selected == true)
			emit bottomCursorPressed(x_tile_16, y_tile_16);
		else
			emit leftMousePressed(x_tile_16, y_tile_16);
	}
	else if(event->button() == Qt::RightButton)
	{
		emit rightMousePressed(x_tile_16, y_tile_16);
	}
}

void widget_gl_gfx_editor::mouseMoveEvent(QMouseEvent * event)
{
	int x_tile_16 = (event->x() + ((scroll_x * 4 * scale))) / (16 * scale);
	int y_tile_16 = (event->y() + ((scroll_y * 4 * scale))) / (16 * scale);

	// Only emit a 'mouseDragged' signal if the cursor went to a different tile.
	if((previous_mouse_x == x_tile_16) && (previous_mouse_y == y_tile_16))
		return;

	if(event->buttons() == Qt::LeftButton)
	{
		if(top_cursor_selected == true)
			emit topCursorDragged(x_tile_16, y_tile_16);
		else if(bottom_cursor_selected == true)
			emit bottomCursorDragged(x_tile_16, y_tile_16);
		else
			emit leftMouseDragged(x_tile_16, y_tile_16);
	} else if(event->buttons() == Qt::RightButton)
	{
		emit rightMouseDragged(x_tile_16, y_tile_16);
	}

	previous_mouse_x = x_tile_16;
	previous_mouse_y = y_tile_16;
}

void widget_gl_gfx_editor::mouseReleaseEvent(QMouseEvent * event)
{
	int x_tile_16 = (event->x() + ((scroll_x * 4 * scale))) / (16 * scale);
	int y_tile_16 = (event->y() + ((scroll_y * 4 * scale))) / (16 * scale);

	if(event->button() == Qt::LeftButton)
	{
		if(top_cursor_selected == true)
			emit topCursorReleased(x_tile_16, y_tile_16);
		else if(bottom_cursor_selected == true)
			emit bottomCursorReleased(x_tile_16, y_tile_16);
		else
			emit leftMouseReleased(x_tile_16, y_tile_16);
	}
	else if(event->button() == Qt::RightButton)
	{
		emit rightMouseReleased(x_tile_16, y_tile_16);
	}

	top_cursor_selected = false;
	bottom_cursor_selected = false;
}

void widget_gl_gfx_editor::set_selection_cursor_position(int x, int y)
{
	selection_cursor_x = x;
	selection_cursor_y = y;
	update_matrices();
	//update();
}

void widget_gl_gfx_editor::set_resize_cursor_position(int x, int y)
{
	resize_cursor_x = x;
	resize_cursor_y = y;
	update_matrices();
	//update();
}

int widget_gl_gfx_editor::get_visible_tiles_width() const
{
	return width / (8 * scale) + 1;
}

int widget_gl_gfx_editor::get_visible_tiles_height() const
{
	return height / (8 * scale) + 1;
}

int widget_gl_gfx_editor::get_visible_tiles_x() const
{
	return scroll_x / 2;
}

int widget_gl_gfx_editor::get_visible_tiles_y() const
{
	return scroll_y / 2;
}

//void widget_gl_gfx_editor::keyPressEvent( QKeyEvent* e ){}
