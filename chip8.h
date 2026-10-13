#ifndef CHIP8_H
#define CHIP8_H

#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>

#include <SDL2/SDL.h>

typedef struct {
    uint16_t stack[16]; // stack frames
    uint16_t pc; // program counter
    uint16_t I; // I register
    uint8_t memory[0x1000]; //4KB memory
    uint8_t reg[16]; // V0 - VF
    uint8_t delay;
    uint8_t sound;
    uint8_t sp; // stack pointer
    uint8_t display[64][32];
    SDL_Window *window; // add sdl stuffs here for easier management
    SDL_Renderer *renderer;
} chip8;

void ld_kk(chip8 *sys, uint8_t reg, uint8_t value);
void add_reg_kk(chip8 *sys, uint8_t reg, uint8_t value);
void call_nnn(chip8 *sys, uint16_t value);
void ret(chip8 *sys);
void jmp(chip8 *sys, uint16_t address);
void cls(chip8 *sys);
void draw(chip8 *sys, uint8_t reg_x, uint8_t reg_y, uint8_t bytes);
void x_or(chip8 *sys, uint8_t reg_x, uint8_t reg_y);
void ld_i_nnn(chip8 *sys, uint16_t bytes);


#endif