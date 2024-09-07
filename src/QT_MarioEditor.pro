#-------------------------------------------------
#
# Project created by QtCreator 2016-07-26T17:18:43
#
#-------------------------------------------------

QT       += core gui
QT       += opengl

QMAKE_CXXFLAGS += -std=c++11

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = QT_MarioEditor
TEMPLATE = app


SOURCES += \
	main.cpp\
	window_main.cpp \
    widget_gl_level_editor.cpp \
    ui_rendering/utilities/shaders.cpp \
    ui_rendering/utilities/textures.cpp \
    load_rom/load_GFX.cpp \
    load_rom/load_rom.cpp \
    load_rom/utilities/load_file.cpp \
    load_rom/utilities/header_test.cpp \
    load_rom/utilities/compression.cpp \
    utilities/buffer.cpp \
    load_rom/load_level.cpp \
    format_rom/format_level.cpp \
    widget_gl_gfx_editor.cpp \
    window_gfx_editor.cpp \
    load_rom/load_palette.cpp \
    format_rom/format_palette.cpp \
    format_rom/format_map16.cpp \
    load_rom/utilities/emulate_object.cpp \
    window_help.cpp \
    format_rom/format_object.cpp \
    load_rom/load_map16.cpp \
    display_rom/display_level.cpp \
    ui_rendering/utilities/hex_spinbox.cpp \
    emulate_rom/emulator.cpp \
    format_rom/format_gfx.cpp \
    animate_rom/animate_gfx.cpp \
    load_rom/load_object_list.cpp \
    format_rom/format_object_list.cpp \
    utilities/address.cpp \
    format_rom/format_level/format_level_object.cpp \
    editor_ui/level_editor_widget.cpp \
    ui_rendering/utilities/fade_label.cpp \
    load_rom/load_object.cpp \
    save_rom/encode_level.cpp \
    format_rom/format_level/format_level_screen_exit.cpp \
    save_rom/find_freespace.cpp \
	load_rom/allocation.cpp \
    format_rom/format_level/format_level_sprite.cpp \
    editor_ui/gfx_editor_widget.cpp \
    editor_ui/rats_viewer_widget.cpp \
    editor_ui/level_list_viewer_widget.cpp

HEADERS  += window_main.h \
    widget_gl_level_editor.h \
    main.h \
    ui_rendering/utilities/shaders.h \
    ui_rendering/utilities/textures.h \
    load_rom/load_GFX.h \
    load_rom/load_rom.h \
    load_rom/definitions_rom.h \
    load_rom/utilities/load_file.h \
    load_rom/utilities/header_test.h \
    load_rom/utilities/compression.h \
    utilities/buffer.h \
    load_rom/load_level.h \
    format_rom/format_level.h \
    format_rom/format_includes.h \
    load_rom/load_includes.h \
    widget_gl_gfx_editor.h \
    window_gfx_editor.h \
    load_rom/load_palette.h \
    format_rom/format_palette.h \
    format_rom/format_map16.h \
    load_rom/utilities/emulate_object.h \
    window_help.h \
    format_rom/format_object.h \
    load_rom/load_map16.h \
    display_rom/display_includes.h \
    display_rom/display_level.h \
    animate_rom/animate_includes.h \
    ui_rendering/utilities/hex_spinbox.h \
    emulate_rom/emulator.h \
    format_rom/format_gfx.h \
    animate_rom/animate_gfx.h \
    load_rom/load_object_list.h \
    format_rom/format_object_list.h \
    error_rom/error_includes.h \
    error_rom/error_definitions.h \
    format_rom/format_enums.h \
    utilities/address.h \
    format_rom/format_level/format_level_object.h \
    format_rom/format_level/format_level_includes.h \
    editor_ui/level_editor_widget.h \
    ui_rendering/utilities/fade_label.h \
    save_rom/encode_level.h \
    format_rom/format_level/format_level_screen_exit.h \
    save_rom/find_freespace.h \
    load_rom/allocation.h \
    utilities/queue.h \
    format_rom/format_level/format_level_sprite.h \
    ui_rendering/utilities/level_rendering.h \
    editor_ui/gfx_editor_widget.h \
    editor_ui/ui_enums.h \
    editor_ui/rats_viewer_widget.h \
    editor_ui/level_list_viewer_widget.h

FORMS    += window_main.ui \
    window_gfx_editor.ui \
    window_help.ui
