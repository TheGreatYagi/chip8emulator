#ifndef DISPLAY_H
#define DISPLAY_H

#include "chip8.h"

// standup sdl
bool init_display(chip8 *sys);

// directly dump display registers to SDL window
void render_display(chip8 *sys);

// teardown sdl
void cleanup_display(chip8 *sys);

#endif