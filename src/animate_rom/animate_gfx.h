#ifndef ANIMATE_PALETTE_H
#define ANIMATE_PALETTE_H

#include "main.h"

void upload_vanilla_gfx_animation(object_emulator & animation_emulator, const SMW_ROM &rom);

void update_vanilla_gfx_animation(object_emulator & animation_emulator, unsigned char frame_counter);

#endif // ANIMATE_PALETTE_H
